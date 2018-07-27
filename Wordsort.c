#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define C_FLAG 		0x01
#define R_FLAG 		0x02
#define N_FLAG 		0x04
#define L_FLAG 		0x08
#define S_FLAG	 	0x10
#define A_FLAG	 	0x20
#define U_FLAG	 	0x40
#define ALL_SORT	0x3C
#define MAX_SIZE	128

typedef struct Word 
{
	char *string;
	unsigned int dupe;
	unsigned int length;
	int numberScore;
	int scrabbleScore;
	struct Word *nextWordPointer;
	struct Word *lastWordPointer;
}Word;

void printHelp(void);
unsigned char parseFlags(int argc, const char *argv[]);
Word *findWords(int argc, const char *argv[], const char flags);
Word *newWord(char *word);
void printList(Word *word, const char flags);
void printList_r(Word *word, const char flags);
Word *insertNewWord(Word *word, char *string, const char flags);
int stringcmp(char *wordString, char *string);
int getScrabbleScore(char *string);

int main(int argc, const char *argv[])
{
	unsigned char flags = 0;
	flags = parseFlags(argc, argv);
	if((flags & ALL_SORT) == 0)
	{
		flags |= A_FLAG;
	}
	Word *tree = findWords(argc,argv, flags);
	if((flags & R_FLAG) == R_FLAG)
	{
		printList_r(tree, flags);
	}
	else
	{
		printList(tree, flags);
	}

	return 0;
}

void printHelp(void)
{
	printf("Help Here.\n");
}

unsigned char parseFlags(int argc, const char *argv[])
{
	int rFlagCount = 0;
	unsigned char flags = 0;
	if(argc > 1)
	{
		for(int i = 0; i < argc; i++)
		{
			if(*argv[i] == '-')
			{
				switch(argv[i][1])
				{
					case('r'):
						rFlagCount++;
						break;
					case('n'):
						flags &= ~ALL_SORT;
						flags |= N_FLAG;
						break;
					case('l'):
						flags &= ~ALL_SORT;
						flags |= L_FLAG;
						break;
					case('s'):
						flags &= ~ALL_SORT;
						flags |= S_FLAG;
						break;
					case('a'):
						flags &= ~ALL_SORT;
						flags |= A_FLAG;
						break;
					case('u'):
						flags |= U_FLAG;
						break;
					case('h'):
						printHelp();
						exit(0);
					default:
						printf("Unknown Switch present.\n");
						exit(1);
				}
			}
		}
		if(rFlagCount % 2 == 1)
		{
			flags |= R_FLAG;
		}
	}
	return flags;
}

Word *findWords(int argc, const char *argv[], const char flags)
{
	FILE *currFile;
	int multiplier = 1;
	int buffSize = MAX_SIZE;
	char *tokens;
	char *buff = calloc((buffSize * multiplier), 1);
	if(buff == NULL)
	{
		printf("Out of memory. Quitting.\n");
		exit(2);
	}
	Word *list = NULL;
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(*argv[i] != '-')
			{
				currFile = fopen(argv[i], "r");
				if(currFile == NULL)
				{
					printf("Could not open file: %s\n", argv[i]);
					continue;
				}
				int j = 0;
				while(fgets(buff, (buffSize * multiplier), currFile) != NULL)
				{
					if(strlen(buff) == (unsigned )(buffSize * multiplier) - 1)
					{
						multiplier++;
						fseek(currFile, -strlen(buff), SEEK_CUR);
						buff = realloc(buff, (buffSize * multiplier)+ 1);	
						continue;
					}
					tokens = strtok(buff, " ");
					while(tokens !=  0)
					{
						if(j == 0)
						{
							list = insertNewWord(list, tokens, flags);
							j++;
						}
						else
						{
							insertNewWord(list, tokens, flags);
						}
						tokens = strtok(NULL, " ");
					}
				}
				fclose(currFile);
				free(buff);
			}
		}
	}
	return list;
}

Word *newWord(char *word)
{
	Word *new = malloc(sizeof(Word));
	word[strlen(word)] = 0;
	if(new == NULL)
	{
		printf("Out of Memory. Quitting.\n");
		exit(2);
	}
	int sum = 0;
	new->string = malloc(strlen(word) + 1);
	if(new->string == NULL)
	{
		printf("Out of Memory. Quitting.\n");
		exit(2);
	}
	strcpy(new->string, word);
	new->length = strlen(word);
	for(unsigned int i = 0; i < strlen(word); i++)
	{
		sum += word[i];	
	}
	new->scrabbleScore = getScrabbleScore(new->string);
	new->numberScore = sum;
	new->nextWordPointer = new->lastWordPointer = NULL;
	return new;
}

void printList(Word *word, const char flags)
{
	if(word != NULL)
	{
		printList(word->lastWordPointer, flags);
		if((flags & U_FLAG) == U_FLAG && (word->dupe & 1) == 1);
		else
		{
			printf("%s\n", word->string);
		}
		printList(word->nextWordPointer, flags);
		free(word->string);
		free(word);
	}
}

void printList_r(Word *word, const char flags)
{
	if(word != NULL)
	{

		printList_r(word->nextWordPointer, flags);
		if((flags & U_FLAG) == U_FLAG && (word->dupe & 1) == 1);
		else{
			printf("%s\n", word->string);
		}
		printList_r(word->lastWordPointer, flags);
	}
}

Word *insertNewWord(Word *word, char *string, const char flags)
{
	string[strcspn(string, "\n")] = 0;
	if(word == NULL)
	{
		return newWord(string);
	}
	int score = 0;
	for(unsigned int i = 0; i < strlen(string); i++)
	{
		score += string[i];	
	}
	if((flags & A_FLAG) == A_FLAG && stringcmp(word->string, string) == 1)
	{
		word->lastWordPointer = insertNewWord(word->lastWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	else if((flags & A_FLAG) == A_FLAG)
	{
		word->nextWordPointer = insertNewWord(word->nextWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	else if(((flags & L_FLAG) == L_FLAG) && word->length < strlen(string))
	{
		word->lastWordPointer = insertNewWord(word->lastWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}

	}
	else if((flags & L_FLAG) == L_FLAG)
	{
		word->nextWordPointer = insertNewWord(word->nextWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}

	}
	else if(((flags & N_FLAG) == N_FLAG) && word->numberScore < score)
	{
		word->lastWordPointer = insertNewWord(word->lastWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	else if((flags & N_FLAG) == N_FLAG)
	{
		word->nextWordPointer = insertNewWord(word->nextWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	else if(((flags & S_FLAG) == S_FLAG) && word->scrabbleScore < score)
	{
		word->lastWordPointer = insertNewWord(word->lastWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	else if((flags & S_FLAG) == S_FLAG)
	{
		word->nextWordPointer = insertNewWord(word->nextWordPointer, string, flags);
		if((flags & U_FLAG) == U_FLAG)
		{
			if(strcmp(word->string, string) == 0)
			{
				word->dupe |= 0x1;
			}
			else
			{
				word->dupe = 0;
			}
		}
	}
	
	return word;
}

int stringcmp(char *wordString, char *string)
{
	int returnCode = 0;
	if(strlen(string) < strlen(wordString))
	{
		for(unsigned int i = 0; i < strlen(string); i++)
		{
			if(string[i] < wordString[i])
			{
				returnCode = 1;
				break;
			}		
			else if(string[i] > wordString[i])
			{
				break;
			}
		}
	}
	else if(strlen(string) > strlen(wordString))
	{
		for(unsigned int i = 0; i < strlen(wordString); i++)
		{
			if(string[i] < wordString[i])
			{
				returnCode = 1;
				break;
			}		
			else if(string[i] > wordString[i])
			{
				break;
			}
		}
	}
	return returnCode;
}

int getScrabbleScore(char *string)
{
	int scrabble[] = { 1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
	int score = 0;
	for(unsigned int i = 0; i < strlen(string); i++)
	{
		score += scrabble[string[i] - 'a'];
	}
	printf("%d\n", score);
	return score;
}
