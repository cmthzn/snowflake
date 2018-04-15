#pragma once

#include <map>
#include <ctype.h>
#include "singleton.hpp"
#include "fnv.hpp"
#include "../sdk/classes/clientclass.hpp"

class netvar_manager : public singleton<netvar_manager> {
private:
	std::unordered_map<unsigned int, recv_prop*> var_map;

	void dump_table(recv_table* table) {
		if (table->props_count < 0)
			return;

		for (auto i = 0; i < table->props_count; ++i) {
			recv_prop *prop = &table->props[i];
			if (!prop)
				continue;

			std::string hash_string = table->table_name;
			hash_string.append("->");
			hash_string.append(prop->prop_name);

			auto hash = fnv_hash(hash_string.data());

			if (var_map.find(hash) == var_map.end())
				var_map.insert({ hash, prop });

			if (prop->data_table)
				this->dump_table(prop->data_table);

		}
	}
public:

	void initialize(client_class* client_data) {
		for (auto data = client_data; data; data = data->next_ptr) {
			if (data->recvtable_ptr) {
				this->dump_table(data->recvtable_ptr);
			}
		}
	}

	unsigned short get_offset(unsigned int hash) {
		auto i = var_map.find(hash);
		if (i != var_map.end())
			return var_map[hash]->offset;
	}

	recv_prop* get_prop(unsigned int hash) {
		auto i = var_map.find(hash);
		if (i != var_map.end())
			return var_map[hash];
	}
};

#define offset_fn(type, var, offset) \
		type& var() { \
			return *(type*)(uintptr_t(this) + offset); \
		} 
#define netvar_fn(type, var, hash) \
		type& var() { \
			static auto _offset = netvar_manager::get().get_offset(fnv_hash(hash)); \
			return *(type*)(uintptr_t(this) + _offset); \
		} 
#define netvar_ptr_fn(type, var, hash) \
		type* var() { \
			static auto _offset = netvar_manager::get().get_offset(fnv_hash(hash)); \
			return (type*)(uintptr_t(this) + _offset); \
		} 
#define netvar_offset_fn(type, var, hash, offset) \
		type& var() { \
			static auto _offset = netvar_manager::get().get_offset(fnv_hash(hash)); \
			return *(type*)(uintptr_t(this) + _offset + offset); \
		} 