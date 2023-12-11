#include "LonelyPartyArray.h"
#include <stdio.h>
#include <stdlib.h>

// Creates a LPA from scratch given the number of fragments and their length
LonelyPartyArray *createLonelyPartyArray(int num_fragments, int fragment_length)
{
	int i;
	int numMax = num_fragments * fragment_length;
	LonelyPartyArray *party;

	if (num_fragments <= 0 || fragment_length <= 0)
		return NULL;

	// This section initializes the elements inside of the LPA structure
	party = malloc(sizeof(LonelyPartyArray));
	party->size = 0;
	party->num_fragments = num_fragments;
	party->fragment_length = fragment_length;
	party->num_active_fragments = 0;
	party->fragments = malloc(sizeof(int*) * num_fragments);
	party->fragment_sizes = malloc(sizeof(int) * num_fragments);

	// Checks whether any memory allocation failed
	if (party == NULL || party->fragments == NULL || party->fragment_sizes == NULL){
		return NULL;
	}

	// Initializes each element in each fragment to NULL
	for (i = 0; i < num_fragments; i++)
		party->fragments[i] = NULL;

	for (i = 0; i < num_fragments; i++)
		party->fragment_sizes[i] = 0;

	printf("-> A new LonelyPartyArray has emerged from the void. (capacity: %d, fragments: %d)\n", 
		numMax, num_fragments);
	return party;
}

// Frees all the memory that was allocated inside the passed LPA
LonelyPartyArray *destroyLonelyPartyArray(LonelyPartyArray *party)
{
	int i;

	if (party == NULL)
		return NULL;

	// Frees the memory of every fragment inside the fragments array that isn't NULL
	for (int i = 0; i < party->num_fragments; i++)
		if (party->fragments[i] != NULL)
			free(party->fragments[i]);

	free(party->fragments);
	free(party->fragment_sizes);
	free(party);

	printf("-> The LonelyPartyArray has returned to the void.\n");
	return NULL;
}

// Inserts key into the passed index while avoiding any possible segmentation faults
int set(LonelyPartyArray *party, int index, int key)
{
	int i, j, k;

	if (party == NULL)
	{
		printf("-> Bloop! NULL pointer detected in set().\n");
		return LPA_FAILURE;
	}

	// Determines the fragment and cell that corresponds to the index
	i = index / party->fragment_length;
	j = index % party->fragment_length;

	// Makes sure the passed index is valid	
	if (index < 0 || index > (party->num_fragments * party->fragment_length - 1))
	{
		printf("-> Bloop! Invalid access in set(). (index: %d, fragment: %d, offset: %d)\n", 
			index, i, j);
		return LPA_FAILURE;
	}

	// Allocates memory to the corresponding fragment if it is NULL
	// Loops through the fragment initializing every cell to UNUSED
	if (party->fragments[i] == NULL)
	{
		party->fragments[i] = malloc(sizeof(int) * party->fragment_length);

		if (party->fragments[i] == NULL)
		{
			return LPA_FAILURE;
		}

		party->num_active_fragments++;

		for (k = 0; k < party->fragment_length; k++)
			party->fragments[i][k] = UNUSED;

		printf("-> Spawned fragment %d. (capacity: %d, indices: %d..%d)\n", 
			i, party->fragment_length, party->fragment_length * i, (party->fragment_length) * (i + 1) - 1);
	}

	// Assigns the value of key to the corresponding cell 
	if (party->fragments[i][j] == UNUSED)
	{
		party->fragments[i][j] = key;
		party->size++;
		party->fragment_sizes[i]++;
	} else {
		party->fragments[i][j] = key;
	}

	return LPA_SUCCESS;
}

// Returns the value in the array at the passed index and avoids any segmentation faults
int get(LonelyPartyArray *party, int index)
{
	int i, j;

	if (party == NULL)
	{
		printf("-> Bloop! NULL pointer detected in get().\n");
		return LPA_FAILURE;
	}

	i = index / party->fragment_length;
	j = index % party->fragment_length;

	if (index < 0 || index > (party->num_fragments * party->fragment_length - 1))
	{
		printf("-> Bloop! Invalid access in get(). (index: %d, fragment: %d, offset: %d)\n", 
			index, i, j);
		return LPA_FAILURE;
	}

	if (party->fragments[i] == NULL)
		return UNUSED;

	return party->fragments[i][j];
}

// Assigns UNUSED to the cell at the passed index 
int delete(LonelyPartyArray *party, int index)
{
	int i, j;

	if (party == NULL)
	{
		printf("-> Bloop! NULL pointer detected in delete().\n");
		return LPA_FAILURE;
	}

	i = index / party->fragment_length;
	j = index % party->fragment_length;

	if (index < 0 || index > (party->num_fragments * party->fragment_length - 1))
	{
		printf("-> Bloop! Invalid access in delete(). (index: %d, fragment: %d, offset: %d)\n", 
			index, i, j);
		return LPA_FAILURE;
	}

	if (party->fragments[i] == NULL)
		return LPA_FAILURE;

	if (party->fragments[i][j] == UNUSED)
		return LPA_FAILURE;

	// Adjusts values inside of the party LPA
	party->fragments[i][j] = UNUSED;
	party->size--;
	party->fragment_sizes[i]--;

	// Deallocates the memory of the fragment if it is empty
	if (party->fragment_sizes[i] == 0)
	{
		free(party->fragments[i]);
		party->fragments[i] = NULL;
		party->num_active_fragments--;
		printf("-> Deallocated fragment %d. (capacity: %d, indices: %d..%d)\n", 
			i, party->fragment_length, party->fragment_length * i, (party->fragment_length) * (i + 1) - 1);
	}

	return LPA_SUCCESS;
}

// Loops through the party LPA and checks if it contains key and avoids any segmentation faults
int containsKey(LonelyPartyArray *party, int key)
{	
	int i, j;

	if (party == NULL)
		return 0;

	for (i=0; i<party->num_fragments; i++)
	{
		if (party->fragments[i] == NULL)
			continue;

		for (j=0; j<party->fragment_length; j++)
		{
			if (party->fragments[i][j] == key)
				return 1;
		}
	}

	return 0;
}

// Checks if there is a value besides UNUSED stored in the party LPA at the index 
// Avoids any segmentation faults
int isSet(LonelyPartyArray *party, int index)
{
	int i, j;

	if (party == NULL)
		return 0;

	i = index / party->fragment_length;
	j = index % party->fragment_length;

	if (index < 0 || index > (party->num_fragments * party->fragment_length - 1))
		return 0;

	if (party->fragments[i] == NULL)
		return 0;

	if (party->fragments[i][j] == UNUSED)
		return 0;

	return 1;
}

// Prints the value stored at the index if the index is valid
int printIfValid(LonelyPartyArray *party, int index)
{
	int i, j;

	if (party == NULL)
		return LPA_FAILURE;

	i = index / party->fragment_length;
	j = index % party->fragment_length;

	if (index < 0 || index > (party->num_fragments * party->fragment_length - 1))
		return LPA_FAILURE;

	if (party->fragments[i] == NULL)
		return LPA_FAILURE;

	if (party->fragments[i][j] == UNUSED)
		return LPA_FAILURE;

	printf("%d\n", party->fragments[i][j]);
	return LPA_SUCCESS;
}

// Resets the passed LPA to its state right after it was created
LonelyPartyArray *resetLonelyPartyArray(LonelyPartyArray *party)
{
	int i;
	int numMax = party->num_fragments * party->fragment_length;

	if (party == NULL)
	{
		printf("-> Bloop! NULL pointer detected in resetLonelyPartyArray().\n");
		return party;
	}

	// Frees any memory that was allocated in the fragments array
	for (i = 0; i < party->num_fragments; i++){
		if (party->fragments[i] != NULL){
			free(party->fragments[i]);
			party->fragments[i] = NULL;
			party->fragment_sizes[i] = 0;
		}
	}

	party->size = 0;
	party->num_active_fragments = 0;
	
	printf("-> The LonelyPartyArray has returned to its nascent state. (capacity: %d, fragments: %d)\n", 
		numMax, party->num_fragments);
	return NULL;
}

// Returns the number of used cells inside of the passed LPA array
int getSize(LonelyPartyArray *party)
{
	if (party == NULL)
		return -1;

	return party->size;
}

// Returns the maximum capacity of the passed LPA
int getCapacity(LonelyPartyArray *party)
{
	if (party == NULL)
		return -1;

	return party->num_fragments * party->fragment_length;
}

// Returns the maximum capacity of the passed LPA without allocating any new fragments
int getAllocatedCellCount(LonelyPartyArray *party){
	if (party == NULL)
		return -1;

	return party->num_active_fragments * party->fragment_length;
}

// Returns the size in bytes the passed LPA would occupy as a regular array
long long unsigned int getArraySizeInBytes(LonelyPartyArray *party)
{
	int numElements;

	if (party == NULL)
		return 0;

	numElements = party->num_fragments * party->fragment_length;

	return (long long unsigned int)numElements * (long long unsigned int)sizeof(int);
}

// Returns the current size in bytes the passed LPA is occupying in memory
long long unsigned int getCurrentSizeInBytes(LonelyPartyArray *party)
{
	int LPApointer, LPAstruct, fragmentsArray, fragmentSizesArray, activeFragments, total, numActiveElements;

	if (party == NULL)
		return 0;

	numActiveElements = party->num_active_fragments * party->fragment_length;

	// Calculates the size in bytes of every element inside of the LPA
	LPApointer = sizeof(LonelyPartyArray*);
	LPAstruct = sizeof(LonelyPartyArray);
	fragmentsArray =  party->num_fragments * (long long unsigned int)sizeof(int*);
	fragmentSizesArray = party->num_fragments * (long long unsigned int)sizeof(int);
	activeFragments = numActiveElements * (long long unsigned int)sizeof(int);
	total = LPApointer + LPAstruct + fragmentsArray + fragmentSizesArray + activeFragments;

	return (long long unsigned int)total;
}

double difficultyRating(void)
{
	return 2.8;
}

double hoursSpent(void)
{
	return 10;
}

// Creates a copy LPA of the passed LPA 
LonelyPartyArray *cloneLonelyPartyArray(LonelyPartyArray *party)
{
	int i, j, k;
	int numMax = party->num_fragments * party->fragment_length;
	int num_fragments = party->num_fragments;
	int fragment_length = party->fragment_length;
	LonelyPartyArray *cloneParty;

	if (party == NULL)
		return NULL;

	// This section initializes the elements inside of the clone LPA structure
	cloneParty = malloc(sizeof(LonelyPartyArray));
	cloneParty->size = 0;
	cloneParty->num_fragments = num_fragments;
	cloneParty->fragment_length = fragment_length;
	cloneParty->num_active_fragments = 0;
	cloneParty->fragments = malloc(sizeof(int*) * num_fragments);
	cloneParty->fragment_sizes = malloc(sizeof(int) * num_fragments);

	// Checks whether any memory allocation failed
	if (cloneParty == NULL || cloneParty->fragments == NULL || cloneParty->fragment_sizes == NULL){
		return NULL;
	}

	// Initializes each element in each fragment to NULL
	for (k = 0; k < num_fragments; k++)
		cloneParty->fragments[k] = NULL;

	for (k = 0; k < num_fragments; k++)
		cloneParty->fragment_sizes[k] = 0;

	// The following section copies the elements of the party LPA into the clone LPA
	cloneParty->size = party->size;
	cloneParty->num_active_fragments = party->num_active_fragments;

	for (i = 0; i<party->num_fragments; i++)
	{
		if (party->fragments[i] != NULL){
			cloneParty->fragments[i] = malloc(sizeof(int) * party->fragment_length);

			if (cloneParty->fragments[i] == NULL)
				return NULL;

			for (j = 0; j<party->fragment_length; j++){
				cloneParty->fragments[i][j] = party->fragments[i][j];
			}
		}

		cloneParty->fragment_sizes[i] = party->fragment_sizes[i];
	}

	printf("-> Successfully cloned the LonelyPartyArray. (capacity: %d, fragments: %d)\n",
		numMax, party->num_fragments);

	return cloneParty;
}

int main(void){
	return 0;
}
