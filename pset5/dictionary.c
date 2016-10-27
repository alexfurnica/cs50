/**
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 */

#include "dictionary.h"

int size_counter = 0;

node* hashtable[HASH_SIZE] = {NULL};

unsigned long hash(const char *str) // djb2 hashing algorithm http://www.cse.yorku.ca/~oz/hash.html
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash =/* ((hash << 5) + hash) + c; */ hash * 33 + c;

    return hash % HASH_SIZE;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // Hash table version
    
    char* temp = malloc(sizeof(char) * (LENGTH + 1));
    
    temp = strcpy(temp, word);
    
    allLower(temp);
    
    unsigned long hword = hash(temp);
    
    if(hashtable[hword] == NULL) // if no pointer in table position, then word clearly does not exist
    {
        return false;
    }
    else
    {
        node* current = hashtable[hword]; // traversal pointer for linked list
        
        while(current != NULL) // run loop until pointer reaches end of linked list
        {
            if(!strcmp(word, current->word)) // if strcmp returns a match (0) then the word exists in the dictionary
            {
                return true;
            }
            else // otherwise go to the next node
            {
                current = current->next;
            }
        }
    }
    
    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // Hash table version
    
    char dict[35] = "";
    
    sprintf(dict, "%s.txt", dictionary);
    
    FILE* file = fopen(dictionary, "r"); // open file
    
    int counter = 0; // counter to see if any words were found
    
    char t;
    
    while((t = fgetc(file)) != EOF) // loop checking that end of file is not reached
    {
        if(!isalpha(t)) // if not alphabetic then skip iteration
        {
            continue;
        }
        
        ungetc(t, file); // return file position indicator back to counteract getc()
        
        node* new_node = malloc(sizeof(node)); // allocate memory for new node
        
        new_node->next = NULL; // initialize next pointer as NULL to identify end of linked list in future functions
        
        fscanf(file, "%s", new_node->word); // get word from file into node
        
        unsigned long hword = hash(new_node->word); // store hash value in a variable for clean code
        
        if(hashtable[hword] == NULL) // if pointer does not point to nothing then it can point to our new node
        {
           hashtable[hword] = new_node;
           
           counter++;
        }
        else // else it should traverse the linked list and insert the new node at the front
        {
            node* current = hashtable[hword]; // point pointer to head of linked list
            
            new_node->next = current; // new node points to old head
            
            hashtable[hword] = new_node; // pointer in hash table points to new node
            
            counter++;
        }
    }
    
    if(counter)
    {
        size_counter = counter; // make globally known the number of words in the dictionary
        
        return true;
    }
    else
    {
        return false;   
    }
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // TODO
    return size_counter; // size already identified during loading
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // Hash table version
    for (int i = 0; i <= HASH_SIZE; i++)
    {
        if(hashtable[i] == NULL)
        {
            continue;
        }
        else
        {
            node* current = hashtable[i];
            
            while(current != NULL)
            {
                node* temp = current;
                
                current = current->next;
                
                free(temp);
            }
        }
    }
    
    return true;
}

char* allLower(char* word)
{
    for( ;*word;word++) *word = tolower(*word);
    
    return word;
}
