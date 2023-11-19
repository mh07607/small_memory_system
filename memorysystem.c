#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEMORYSIZE 500
#define MAXSTACKSIZE 300
#define MAXHEAPSIZE 300
#define MAXFRAMES 5
#define MINFRAMESIZE 10
#define MAXFRAMESIZE 80
#define STACKSTARTINGINDEX 394
#define HEAPSTARTINGINDEX 0


typedef struct allocation {
	char name[8];
	int frameaddress;
	struct allocation * next;
} allocation;

typedef struct framestatus {		  // 21 bytes
	int number;               // frame number
    char name[8];             // function name representing the frame
	int functionaddress;      // address of function in code section (will be randomly generated in this case)
	int frameaddress;         // starting address of frame belonging to this header in Stack
	char used;                 // a boolean value indicating wheter the frame status entry is in use or not
} framestatus;

typedef struct freelist {		//12 bytes
	int start;         // start address of free region
	int size;                 // size of free region
	struct freelist * next;   // pointer to next free region
} freelist;


void create_frame(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size,
char * name,
int address)
{
	if(strlen(name) > 8 || strlen(name) < 1){
		printf("Incorrect name.\n");
		return;
	}
	if(*current_stack_index - MINFRAMESIZE <= *current_heap_size || STACKSTARTINGINDEX - *current_stack_index + MINFRAMESIZE > MAXSTACKSIZE - sizeof(framestatus) * 5){
		printf("Not enough space in stack.\n");
		return;
	}
	int frame_found = 0;
	for(int i = 0; i < MAXFRAMES; i++){
		if(strcmp(framestatus_list[i]->name, name) == 0){
			printf("Function with given name already exists!\n");
			return;
		}

		if(framestatus_list[i]->used == '0'){
			frame_found = 1;
			framestatus_list[i]->used = '1';
			framestatus_list[i]->frameaddress = *current_stack_index;
			strcpy(framestatus_list[i]->name, name);			
			framestatus_list[i]->functionaddress = address;
			break;
		}
	}
	if(!frame_found){
		printf("Not enough space for another frame.\n");
		return;
	}
	// 10 bytes on default allocated for frame
	*current_stack_index = *current_stack_index - MINFRAMESIZE;
	printf("Frame '%s' successfully created!\n", name);
}

void delete_function(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size)
{
	int i = -1; //setting i to -1 because we are checking for i+1 so don't want to go out of range
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	framestatus_list[i]->used = '0';
	*current_stack_index = framestatus_list[i]->frameaddress;
	printf("Frame '%s' successfully deleted!\n", framestatus_list[i]->name);
}


int expand_frame(
framestatus * framestatus_list[MAXFRAMES], 
int * current_stack_index, 
int frame_index, 
int size){
	int frameSize =  framestatus_list[frame_index]->frameaddress - *current_stack_index - size;
	if(frameSize > MAXFRAMESIZE){
		printf("Cannot create variable because maximum frame size (80) will be exceeded. Current frame size: %d, size required: %d.\n", frameSize-size, frameSize);
		return 0;
	}
	*current_stack_index -= size;
	return *current_stack_index;
}


int create_integer(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size,
char * memory,
char * name, 
int value)
{
	//checking if all contraints are met
	int currentFrameSize = STACKSTARTINGINDEX - *current_stack_index;
	if(currentFrameSize + sizeof(int) > MAXSTACKSIZE - sizeof(framestatus) * 5 || *current_stack_index - sizeof(int) <= *current_heap_size){
		printf("Not enough space on stack!\n");
		return 0;
	}

	int i = -1; //setting i to -1 because we are checking for i+1 so don't want to go out of range
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return 0;
	}
	int variableCreationIndex = expand_frame(framestatus_list, current_stack_index, i, sizeof(int));
	if(!variableCreationIndex){
		return 0;
	}
	
	//Now that all checks have been made, we can safely create integer
	fflush(stdin);
	memcpy(&memory[variableCreationIndex], &value, sizeof(int));
	printf("Integer '%s' = %d sucessfully created.\n", name, value);

	return 1;
}

void create_double(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size,
char * memory,
char * name, 
double value)
{
	//checking if all contraints are met
	int currentFrameSize = STACKSTARTINGINDEX - *current_stack_index;
	if(currentFrameSize + sizeof(double) > MAXSTACKSIZE - sizeof(framestatus) * 5 || *current_stack_index - sizeof(double) <= *current_heap_size){
		printf("Not enough space on stack!\n");
		return;
	}

	int i = -1; //setting i to -1 because we are checking for i+1 so don't want to go out of range
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	int variableCreationIndex = expand_frame(framestatus_list, current_stack_index, i, sizeof(double));
	if(!variableCreationIndex){
		return;
	}
	
	//Now that all checks have been made, we can safely create double
	fflush(stdin);
	memcpy(&memory[variableCreationIndex], &value, sizeof(double));
	printf("Double '%s' = %f sucessfully created.\n", name, value);
}

void create_character(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size,
char * memory,
char * name, 
char value)
{
	//checking if all contraints are met
	int currentFrameSize = STACKSTARTINGINDEX - *current_stack_index;
	if(currentFrameSize + sizeof(char) > MAXSTACKSIZE - sizeof(framestatus) * 5 || *current_stack_index - sizeof(char) <= *current_heap_size){
		printf("Not enough space on stack!\n");
		return;
	}

	int i = -1; //setting i to -1 because we are checking for i+1 so don't want to go out of range
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	int variableCreationIndex = expand_frame(framestatus_list, current_stack_index, i, sizeof(char));
	if(!variableCreationIndex){
		return;
	}
	
	//Now that all checks have been made, we can safely create character
	fflush(stdin);
	memory[variableCreationIndex] = value;
	printf("Character '%s' = %c sucessfully created.\n", name, value);
}

void subtract_free_list_node(freelist ** freelistNode, int size, int address){
	(*freelistNode)->size -= size;
	(*freelistNode)->start = address;
}

void create_buffer(
framestatus * framestatus_list[MAXFRAMES],
allocation ** allocationRoot,
freelist ** freelistRoot,
int * current_stack_index,
int * current_heap_size,
char * memory,
char * name, 
int size)
{
	if(freelistRoot == NULL){
		printf("Not enough space on heap for the allocation.\n");
		return;
	}
	freelist * freeNode = *freelistRoot;
	while(freeNode != NULL){
		if(freeNode->size >= size){
			break;
		}
	}
	if(freeNode == NULL || *current_heap_size + 8 + size >= *current_stack_index){
		printf("Not enough space on heap for the allocation.\n");
		return;
	}

	int bufferIndex = freeNode->start;
	if(!create_integer(framestatus_list, current_stack_index, current_heap_size, memory, name, bufferIndex)){
		printf("Not enough space on stack for storing pointer!\n");
		return;
	}

	subtract_free_list_node(&freeNode, size, bufferIndex+8+size);

	allocation * node;
	if(*allocationRoot == NULL){
		*allocationRoot = malloc(sizeof(allocation));
		node = *allocationRoot;
	}
	else {
		allocation * temp = *allocationRoot;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = malloc(sizeof(allocation));
		node = temp->next;
	}

	strcpy(node->name, name);
	node->frameaddress = bufferIndex;
	node->next = NULL;

	int magicNumber = 69;
	memcpy(&memory[bufferIndex], &size, sizeof(int));
	memcpy(&memory[bufferIndex+4], &magicNumber, sizeof(int));
	//storing dummy data
	for(int i = 0; i < size; i++){
		memory[bufferIndex+8+i] = 'a' + (i%26);
	}

	*current_heap_size += bufferIndex+8+size;
	printf("Buffer '%s' successfully allocated!\n", name);
}

void delete_buffer(
framestatus * framestatus_list[MAXFRAMES],
allocation ** allocationRoot,
freelist ** freelistRoot,
int * current_stack_index,
int * current_heap_size,
char * memory,
char * name
)
{
	allocation * node = *allocationRoot;
	int allocationAddress;

	if(strcmp(node->name, name) == 0){
		allocationAddress = node->frameaddress;

		*allocationRoot = node->next;
		free(node);
	}
	else{
		while(node->next != NULL){
			if(strcmp(node->next->name, name)==0){
				break;
			}
			node = node->next;
		}
		if(node->next == NULL){
			printf("Null pointer or allocation already deallocated.\n");
			return;
		}
	
		allocationAddress = node->next->frameaddress;
		allocation * temp = node->next;
		node->next = node->next->next;
		free(temp);
	}

	int size = 8+*(int*)&memory[allocationAddress];

	//adding a freenode at head of the freelist
	freelist * freenode = malloc(sizeof(freelist));
	freenode->start = allocationAddress;
	freenode->size = size;
	freelist * oldroot = *freelistRoot;
	*freelistRoot = freenode;
	freenode->next = oldroot;

	for(int i = 0; i < size; i++){
		memory[allocationAddress+i] = '0';
	}
	printf("Buffer '%s' successfully deallocated.\n", name);
}

void show_memory(
freelist * freelistRoot,
allocation * allocationRoot,
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_size,
char * memory
)
{
	//Printing stack and heap images
	fflush(stdin);
	printf("----------------Memory Image-------------------\n");
	printf("Framestatuses list:\n");
	for(int i = 0; i < MAXFRAMES; i++){
		if(framestatus_list[i]->used == '0'){
			break;
		}
		printf("Name: %s, Address: %d\n", framestatus_list[i]->name, framestatus_list[i]->frameaddress);
	}
	printf("Stack Image:\n");
	for(int i = STACKSTARTINGINDEX; i > *current_stack_index; i--){
		printf("%c", memory[i]);
	}
	printf("\n");
	printf("Allocations list\n");
	while(allocationRoot != NULL){
		printf("Name: %s, Address: %d, Size: %d\n", allocationRoot->name, allocationRoot->frameaddress,  8+*(int*)&memory[allocationRoot->frameaddress]);
		allocationRoot = allocationRoot->next;
	}
	printf("Freelist: \n");
	while(freelistRoot != NULL){
		if(freelistRoot->size > 0){
			printf("Address: %d, Size: %d\n", freelistRoot->start, freelistRoot->size);
		}
		freelistRoot = freelistRoot->next;
	}
	printf("Heap Image:\n");
	for(int i = HEAPSTARTINGINDEX; i < *current_heap_size; i++){
		printf("%c", memory[i]);
	}
	printf("\n-----------------------------------------------\n");
}

int main (int argc, char * argv[]) {
	int current_stack_index = STACKSTARTINGINDEX;
	int current_heap_size = HEAPSTARTINGINDEX;

	char memory[MEMORYSIZE];
	// Initialize framestasuses
	framestatus * framestatus_list[MAXFRAMES];
	for(int i = 0; i < MAXFRAMES; i++){
		framestatus_list[i] = (framestatus*)(memory + sizeof(memory) - (i+1) * sizeof(framestatus));
		framestatus_list[i]->used = '0';
		framestatus_list[i]->number = i+1;
	}

	//initialize freelist
	freelist * freelistRoot = malloc(sizeof(freelist));
	freelistRoot->next = NULL;
	freelistRoot->size = 300;
	freelistRoot->start = HEAPSTARTINGINDEX;

	allocation * allocationRoot = NULL;

	//--------------------testing code----------------------------
	// create_frame(framestatus_list, &current_stack_index, &current_heap_size, "yeet", 0);
	// create_integer(framestatus_list, &current_stack_index, &current_heap_size, memory, "yeetInt", 69);
	// printf("%d, %d\n", current_stack_index, *(int *)(&memory[current_stack_index]));
	// create_integer(framestatus_list, &current_stack_index, &current_heap_size, memory, "yeetInt", 420);
	// printf("%d, %d\n", current_stack_index, *(int *)(&memory[current_stack_index]));
	// create_integer(framestatus_list, &current_stack_index, &current_heap_size, memory, "yeetInt", 1000);
	// printf("%d, %d\n", current_stack_index, *(int *)(&memory[current_stack_index]));
	// create_double(framestatus_list, &current_stack_index, &current_heap_size, memory, "yeetdub", 3.3);
	// printf("%d, %f\n", current_stack_index, *(double *)(&memory[current_stack_index]));
	// create_character(framestatus_list, &current_stack_index, &current_heap_size, memory, "yeetchar", 'a');
	// printf("%d, %c\n", current_stack_index, *(char *)(&memory[current_stack_index]));

	// create_buffer(framestatus_list, &allocationRoot, &freelistRoot, &current_stack_index, &current_heap_size, memory, "ying", 50);
	// create_buffer(framestatus_list, &allocationRoot, &freelistRoot, &current_stack_index, &current_heap_size, memory, "yong", 50);
	// delete_buffer(framestatus_list, &allocationRoot, &freelistRoot, &current_stack_index, &current_heap_size, memory, "yong");

	// show_memory(freelistRoot, allocationRoot, framestatus_list, &current_stack_index, &current_heap_size, memory);
	// delete_function(framestatus_list, &current_stack_index, &current_heap_size);
	// printf("%d\n", current_stack_index);
	// ------------------------testing code-----------------------

	char line[256];
	while(fgets(line, sizeof(line), stdin)!=NULL)
	{
	char * result[10]; //maximum 9 arguments
	int i = 0;

	// Remove trailing newline characters
	int length = strlen(line);
	if (length > 0 && line[length - 1] == '\n') {
		line[length - 1] = '\0';
	}

	char * command;
	command = strdup(line); //preserving line in case of error
	char * token;
	token = strtok(command, " ");
	while (token != NULL) {
		result[i] = strdup(token);
		i++;
		token = strtok(NULL, " "); 
	}
	if(strcmp(result[0], "CF")==0){
		create_frame(framestatus_list, &current_stack_index, &current_heap_size, result[1], atoi(result[2]));
	}
	else if(strcmp(result[0], "DF")==0){      
		delete_function(framestatus_list, &current_stack_index, &current_heap_size);
	}
	else if(strcmp(result[0], "CI")==0){
		create_integer(framestatus_list, &current_stack_index, &current_heap_size, memory, result[1], atoi(result[2]));
	}
	else if(strcmp(result[0], "CD")==0){
		char *endptr;
		create_double(framestatus_list, &current_stack_index, &current_heap_size, memory, result[1], strtod(result[2], &endptr));
	}
	else if(strcmp(result[0], "CC")==0){
		if(strlen(result[2]) > 1){
			printf("Invalid character.\n");
		}
		else{
			create_character(framestatus_list, &current_stack_index, &current_heap_size, memory, result[1], result[2][0]);
		}
	}
	else if(strcmp(result[0], "CH")==0){
		create_buffer(framestatus_list, &allocationRoot, &freelistRoot, &current_stack_index, &current_heap_size, memory, result[1], atoi(result[2]));
	}
	else if(strcmp(result[0], "DH")==0){
		delete_buffer(framestatus_list, &allocationRoot, &freelistRoot, &current_stack_index, &current_heap_size, memory, result[1]);
	}
	else if(strcmp(result[0], "SM")==0){
		show_memory(freelistRoot, allocationRoot, framestatus_list, &current_stack_index, &current_heap_size, memory);
	}
	else {
		printf("%s is an incorrect Command.\n", line);
	}
	}

	return 0;
}
