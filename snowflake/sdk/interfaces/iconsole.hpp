#pragma once
#include "../classes/convar.hpp"

class i_console {
public:
	convar * get_convar( const char* name ) {
		using original_fn = convar * ( __thiscall* )( i_console*, const char* );
		return ( *( original_fn** ) this ) [ 16 ]( this, name );
	}
};