/**
 * @file      main.c
 * @author    Jeramie Vens
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "json-parser/json.h"
#include "json-builder/json-builder.h"

/**
 * A wavedrom signal
 */
struct wavedrom_sig
{
	/// The name of this signal
	char * name;
	/// The waveform string for this signal
	char * waveform;
	/// Array of data labels
	char ** data;
	/// The max length of the waveform string
	size_t waveform_length;
	/// The max lengto of the data array
	size_t data_length;
	/// The current char index in the waveform string
	int waveform_index;
	/// The current index in the data array
	int data_index;
};

enum node_type{
	NODE_LOW = '0',
	NODE_HIGH = '1',
	NODE_Z = 'z',
	NODE_UP = 'u',
	NODE_DOWN = 'd',
	NODE_CONTINUE = '.',
	NODE_PCLK = 'p',
	NODE_PCLK_ARROW = 'P',
	NODE_NCLK = 'n',
	NODE_NCLK_ARROW = 'N',
	NODE_SPACER = '|'
};

enum node_data_type{
	NODE_WHITE = '2',
	NODE_YELLOW = '3',
	NODE_RED = '4',
	NODE_BLUE = '5'
};

/**
 * Create a new Wavedrom signal
 * @param     name
 *                 The name of this signal
 * @return    The new wavedrom signal object.  This object must be destroyed
 *            using wavedrom_sig_destroy()
 */
struct wavedrom_sig * wavedrom_sig_create(const char * name)
{
	struct wavedrom_sig * sig = calloc(1, sizeof(struct wavedrom_sig));
	sig->name = malloc(strlen(name)+1);
	sig->waveform = calloc(1, 50);
	sig->data = calloc(20, sizeof(char *));
	sig->waveform_length = 50;
	sig->data_length = 20;
	strcpy(sig->name, name);
	return sig;
}

/**
 * Destroy a wavedrom signal created with wavedrom_sig_create()
 * @param     sig
 *                 The signal object to destroy
 */
void wavedrom_sig_destroy(struct wavedrom_sig * sig)
{
	free(sig->name);
	free(sig->waveform);
	free(sig->data);
	free(sig);
}

/**
 * Add a node to the wavedrom signal object
 * @param     sig
 *                 The signal to add the node to
 * @param     node
 *                 The type of node to add
 * @return    sig
 */
struct wavedrom_sig * wavedrom_sig_node(struct wavedrom_sig * sig, enum node_type node)
{
	if(sig->waveform_length <= sig->waveform_index + 1)
	{
		sig->waveform = realloc(sig->waveform, sig->waveform_length + 20);
		sig->waveform_length += 20;
	}
	sig->waveform[sig->waveform_index++] = node;
	sig->waveform[sig->waveform_index] = '\0';
	return sig;
}

/**
 * Add a named node to the wavedrom signal object
 * @param     sig
 *                 The signal to add the node to
 * @param     node
 *                 The type of node to add
 * @param     name
 *                 The name of this node
 * @return    sig
 */
struct wavedrom_sig * wavedrom_sig_named_node(struct wavedrom_sig * sig, enum node_data_type node, const char * name)
{
	wavedrom_sig_node(sig, node);
	if(sig->data_length <= sig->data_index)
	{
		sig->data = realloc(sig->data, sig->data_length + 20);
		sig->data_length += 20;
	}
	sig->data[sig->data_index] = malloc(strlen(name)+1);
	strcpy(sig->data[sig->data_index++], name);
	return sig;
}

/**
 * Continue the current node another clock cycle
 * @param     sig
 *                 The signal to continue
 * @return    sig
 */
struct wavedrom_sig * wavedrom_sig_continue(struct wavedrom_sig * sig)
{
	return wavedrom_sig_node(sig, NODE_CONTINUE);
}

/**
 * Convert a wavedrom signal object into a JSON object
 * @param     sig
 *                 The signal to convert into JSON
 * @return    The new JSON object
 */
json_value* wavedrom_sig_to_json(const struct wavedrom_sig * sig)
{
	int i;
	json_value* sig_obj = json_object_new(0);
	json_value* data = json_array_new(0);
	json_object_push(sig_obj, "name", json_string_new(sig->name));
	json_object_push(sig_obj, "wave", json_string_new(sig->waveform));
	for(i=0;i<sig->data_index;i++)
		json_array_push(data, json_string_new(sig->data[i]));
	json_object_push(sig_obj, "data", data);
	return sig_obj;
}


int main(int argc, char ** argv)
{
	json_value* root = json_object_new(0);

	json_value* signals = json_array_new(0);
	
	struct wavedrom_sig * sig1 = wavedrom_sig_create("Signal 1");
	struct wavedrom_sig * sig2 = wavedrom_sig_create("Signal 2");

	wavedrom_sig_named_node(sig1, NODE_RED, "Data 1");
	wavedrom_sig_continue(sig1);
	wavedrom_sig_continue(sig1);
	wavedrom_sig_continue(sig1);
	wavedrom_sig_node(sig1, NODE_Z);
	wavedrom_sig_continue(sig1);
	wavedrom_sig_named_node(sig1, NODE_YELLOW, "data 2");

	wavedrom_sig_node(sig2, NODE_HIGH);
	wavedrom_sig_continue(sig2);
	wavedrom_sig_continue(sig2);
	wavedrom_sig_continue(sig2);
	wavedrom_sig_continue(sig2);
	wavedrom_sig_node(sig2, NODE_LOW);
	wavedrom_sig_continue(sig2);
	wavedrom_sig_continue(sig2);



	json_value* s1 = wavedrom_sig_to_json(sig1);
	json_value* s2 = wavedrom_sig_to_json(sig2);
	json_array_push(signals, s1);
	json_array_push(signals, s2);
	json_object_push(root, "signal", signals);

	char * buf = malloc(json_measure(root));
	json_serialize(buf, root);
	printf("%s\n", buf);
	return 0;
}
