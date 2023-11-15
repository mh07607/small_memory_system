#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEMORYSIZE 500
#define MAXSTACKSIZE 300
#define MAXHEAPSIZE 300
#define MAXFRAMES 5
#define MINFRAMESIZE 10
#define MAXFRAMESIZE 80
#define STACKSTARTINGINDEX 105
#define HEAPSTARTINGINDEX 499

// typedef struct framestatus_list {
// 	framestatus list[5];
// } framestatus_list;

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
int * current_heap_index,
char * name,
int address)
{
	if(strlen(name) > 8 || strlen(name) < 1){
		printf("Incorrect name.\n");
		return;
	}
	// if(*current_stack_index + MINFRAMESIZE - *current_heap_index > MAXSTACKSIZE){
	// 	printf("Not enough space in stack.\n");
	// 	return;
	// }
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
			framestatus_list[i]->number = i;
			framestatus_list[i]->functionaddress = address;
			break;
		}
	}
	
	if(!frame_found){
		printf("Not enough space for another frame.\n");
		return;
	}

	*current_stack_index += MINFRAMESIZE;
}

void delete_function(
 framestatus * framestatus_list[MAXFRAMES],
 int * current_stack_index,
 int * current_heap_index)
{
	int i = -1; //setting i to -1 because
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	framestatus_list[i]->used = '0';
	*current_stack_index -= (framestatus_list[i]->frameaddress+10);
}

void create_integer(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_index,
char * memory,
char * name, 
int value)
{
	//checking if all contraints are met
	if(*current_stack_index + sizeof(int) - STACKSTARTINGINDEX > MAXSTACKSIZE || *current_stack_index + sizeof(int) >= *current_heap_index){
		printf("Not enough space on stack!\n");
		return;
	}

	int i = -1; //setting i to -1 because
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	int frameSize = *current_stack_index + sizeof(int) - framestatus_list[i]->frameaddress;
	if(frameSize > MAXFRAMESIZE){
		printf("Cannot create integer because maximum frame size (80) will be exceeded. Current frame size: %d\n", frameSize);
		return;
	}
	//Now that all checks have been made, we can safely create integer
	memcpy(&memory[*current_stack_index], &value, sizeof(int));
	*current_stack_index += sizeof(int);
}

void create_double(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_index,
char * memory,
char * name, 
double value){
	//checking if all contraints are met
	if(*current_stack_index + sizeof(double) - STACKSTARTINGINDEX > MAXSTACKSIZE || *current_stack_index + sizeof(double) >= *current_heap_index){
		printf("Not enough space on stack!\n");
		return;
	}

	int i = -1; //setting i to -1 because
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	int frameSize = *current_stack_index + sizeof(double) - framestatus_list[i]->frameaddress;
	if(frameSize > MAXFRAMESIZE){
		printf("Cannot create double because maximum frame size (80) will be exceeded. Current frame size: %d\n", frameSize);
		return;
	}
	//Now that all checks have been made, we can safely create integer
	memcpy(&memory[*current_stack_index], &value, sizeof(double));
	*current_stack_index += sizeof(double);
}

void create_character(framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_index,
char * memory,
char * name, 
char value){
	//checking if all contraints are met
	if(*current_stack_index + sizeof(char) - STACKSTARTINGINDEX > MAXSTACKSIZE || *current_stack_index + sizeof(char) >= *current_heap_index){
		printf("Not enough space on stack!\n");
		return;
	}

	int i = -1; //setting i to -1 because
	while(i < MAXFRAMES-1 && framestatus_list[i+1]->used == '1'){
		i += 1;
	}
	if(i == -1){
		printf("The stack is empty!\n");
		return;
	}
	int frameSize = *current_stack_index + sizeof(char) - framestatus_list[i]->frameaddress;
	if(frameSize > MAXFRAMESIZE){
		printf("Cannot create character because maximum frame size (80) will be exceeded. Current frame size: %d\n", frameSize);
		return;
	}
	//Now that all checks have been made, we can safely create integer
	memory[*current_stack_index] = value;
	*current_stack_index += sizeof(char);
}

void create_buffer(
framestatus * framestatus_list[MAXFRAMES],
int * current_stack_index,
int * current_heap_index,
char * memory,
char * name, 
int size)
{
	for(int i = size; i > -1; i--){
		memory[*current_heap_index-i] = ('a'+i) % 26;
	}
	*current_heap_index -= size + 4;
}

void delete_buffer(char * name){

}

void show_memory(
int * current_stack_index,
int * current_heap_index,
char * memory
)
{
	//Printing stack and heap images
	printf("Stack Image:\n");
	for(int i = STACKSTARTINGINDEX; i < *current_stack_index; i++){
		printf("%c", memory[i]);
	}
	printf("\nHeap Image:\n");
	for(int i = HEAPSTARTINGINDEX; i > *current_heap_index; i--){
		printf("%c", memory[i]);
	}
	printf("\n");
}

int main () {
	int current_stack_index = STACKSTARTINGINDEX;
	int current_heap_index = HEAPSTARTINGINDEX;

	//char * memory = malloc(MEMORYSIZE);
	char memory[MEMORYSIZE];
	// Initialize framestasuses
	framestatus * framestatus_list[MAXFRAMES];
	for(int i = 0; i < MAXFRAMES; i++){
		framestatus_list[i] = (framestatus*)(memory + i * sizeof(framestatus));
		framestatus_list[i]->used = '0';
	}
	
	create_frame(framestatus_list, &current_stack_index, &current_heap_index, "yeet", 0);
	create_integer(framestatus_list, &current_stack_index, &current_heap_index, memory, "yeetInt", 69);
	create_double(framestatus_list, &current_stack_index, &current_heap_index, memory, "yeetDouble", 3.3);
	printf("%d, %d\n", current_stack_index, *(int *)(&memory[current_stack_index-sizeof(double)-sizeof(int)]));
	printf("%d, %f\n", current_stack_index, *(double *)(&memory[current_stack_index-sizeof(double)]));

	return 0;
}
