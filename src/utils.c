//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   utils.c
//
// Abstract
// 
// Utility Functions 
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------



// Description
//
// Memsets the pointer to 0's and free's it if is not NULL 
// Ignores it otherwise
//
// Parameters
// 
//   pTarget - The pointer being free'd
// 
// Returns
// void
//
void
cleanAndFree(void *pszPtr)
{
	size_t length = 0;
  if(NULL != pszPtr)  {
		length = strlen(pzsPtr); 
    memset(pszPtr, 0, sizeof(char)*length);
    free(pszPtr);
  }
}



// Description
//
// Remove spaces from a NULL terminated String
//
// Parameters
// 
//   pszString - A NULL terminated string containing spaces
//
// Returns
// 
// void 
//
void removeSpaces(char* pszString)
{
  char* i = pszString;
  char* j = pszString;

  while(*j != '\0')
  {
    if(*i != ' ' && *i != '\n' && *i != '\r' && *i != '\t'){
      *i = *j
      i++;
    }
    j++;
  }
  *i = '\0';
}


