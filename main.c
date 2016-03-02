

#include <stdio.h>
#include <stdlib.h>

#include "json-parser/json.h"
#include "json-builder/json-builder.h"

json_value* make_signal(const char * name, const char * wave)
{
	json_value* sig = json_object_new(0);
	json_object_push(sig, "name", json_string_new(name));
	json_object_push(sig, "wave", json_string_new(wave));
	return sig;
}


int main(int argc, char ** argv)
{
	json_value* root = json_object_new(0);

	json_value* signals = json_array_new(0);
	
	json_value* s1 = make_signal("Sig 1", "1...2...3...");
	json_value* s2 = make_signal("Sig 2", "2...1...x...");
	json_array_push(signals, s1);
	json_array_push(signals, s2);
	json_object_push(root, "signal", signals);

	char * buf = malloc(json_measure(root));
	json_serialize(buf, root);
	printf("%s\n", buf);
	return 0;
}
