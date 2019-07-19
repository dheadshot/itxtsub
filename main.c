#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

ntsa txtimg;
int width = 0, height = 0;
ntsa subs = NULL;

void freentsa(ntsa a)
{
  if (!a) return;
  int i;
  for (i=0; a[i] != NULL; i++) free(a[i]);
  free(a);
}

void freesubs()
{
  freentsa(subs);
}

void freetxtimg()
{
  freentsa(txtimg);
}

int loadtxtimg(char *afn)
{
  char buffer[MAXBUFFER] = "";
  if (!afn) return 0;
  FILE *afp = fopen(afn, "r");
  if (!afp)
  {
    fprintf(stderr, "Could not read \"%s\"!\n", afn);
    return -1;
  }
  int n = 0;
  while (!feof(afp))
  {
    if (!fgets(buffer,MAXBUFFER,afp)) break;
    if (buffer[0] != 0)n++;
  }
  if (!feof(afp))
  {
    fprintf(stderr, "Error reading \"%s\"!\n", afn);
    fclose(afp);
    return -2;
  }
  fclose(afp);
  if (!n)
  {
    fprintf(stderr, "File \"%s\" is empty!\n", afn);
    return -3;
  }
  
  txtimg = (ntsa) malloc(sizeof(char *) * (n+1));
  if (!txtimg)
  {
    fprintf(stderr, "Out of Memory!\n");
    return -1;
  }
  memset(txtimg,0,sizeof(char *)*(n+1));
  
  afp = fopen(afn, "r");
  if (!afp)
  {
    fprintf(stderr, "Could not read \"%s\"!\n", afn);
    free(txtimg);
    txtimg = NULL;
    return -1;
  }
  
  int i, l;
  for (i=0; i<n; i++)
  {
    if (!fgets(buffer,MAXBUFFER,afp)) break;
    if (buffer[0]!=0)
    {
      l = strlen(buffer);
      if (buffer[l-1] == '\n')
      {
        buffer[l-1] = 0;
        l--;
      }
      if (buffer[l-1] == '\r')
      {
        buffer[l-1] = 0;
        l--;
      }
      txtimg[i] = (char *) malloc(sizeof(char)*(l+1));
      if (!txtimg[i])
      {
        for (l=0;l<i;l++)
        {
          free(txtimg[l]);
        }
        free(txtimg);
        txtimg = NULL;
        fclose(afp);
        fprintf(stderr, "Out of Memory!\n");
        return -1;
      }
      strcpy(txtimg[i], buffer);
    }
  }
  txtimg[i] = NULL;
  height = n;
  
  fclose(afp);
  
  return 1;
}

void gettxtimgstats()
{
  int i, n, m;
  n = 0;
  for (i=0; txtimg[i]!=NULL; i++)
  {
    m = strlen(txtimg[i]);
    if (m>n) n = m;
  }
  height = i;
  width = n;
#ifdef DEBUG
  printf("%dx%d\n", width, height);
#endif
}

int makesubs(char *substring)
{
  int n, i, j, m, k = 0;
  if (!substring) return 0;
  if (subs)
  {
    fprintf(stderr, "Already have subs!\n");
    return -1;
  }
  n = 1;
  if (substring[strlen(substring)-1] == '\n') n = 0;
  for (i=0; substring[i] != 0; i++)
  {
    if (substring[i] == '\n') n++;
  }
  
#ifdef DEBUG
  printf("%d subs\n", n);
#endif
  
  subs = (ntsa) malloc(sizeof(char *) * (n+1));
  if (!subs)
  {
    fprintf(stderr, "Out of Memory!\n");
    return -2;
  }
  memset(subs, 0, sizeof(char *)*(n+1));
  
  char buffer[MAXBUFFER] = "";
  m = 0;
  j = 0;
  for (i=0; substring[i]!=0; i++)
  {
    if (substring[i]!='\r')
    {
      if (substring[i]!='\t') buffer[j] = substring[i];
      else buffer[j] = ' ';
      j++;
    }
    if (substring[i]=='\n')
    {
      subs[m] = (char *) malloc(sizeof(char)*(1+j));
      if (!subs[m])
      {
        for (j=0;j<m;j++)
        {
          free(subs[j]);
        }
        free(subs);
        subs = NULL;
        fprintf(stderr, "Out of Memory!\n");
        return -2;
      }
      buffer[j-1] = 0;
      strcpy(subs[m], buffer);
      
      m++;
      j = 0;
      buffer[0] = 0;
    }
  }
  return 1;
}

int addsubs()
{
  int longestsub = 0, numsubs, n, i, j, indent, padding = 0, k;
  char buffer[MAXBUFFER];
  if (!subs || !txtimg) return 0;
  for (numsubs = 0; subs[numsubs] != NULL; numsubs++)
  {
    n = strlen(subs[numsubs]);
    if (n>longestsub) longestsub = n;
  }
  
  if (numsubs > height)
  {
    fprintf(stderr, "Too many subtitles for this screen!\n");
    return -1;
  }
  if ((longestsub << 1) > width)
  {
    fprintf(stderr, "Subtitles too wide for this screen!\n");
    return -2;
  }
  
  indent = (width - (longestsub << 1)) >> 1;
  if ((indent & 1) != 0) indent--;
  if (indent > 1)
  {
    padding = 1;
    indent-=2;
  }
  
  for (i=0; i<numsubs; i++)
  {
    for (j=0; subs[i][j] != 0; j++)
    {
      buffer[(j<<1)] = 0x8F;
      buffer[(j<<1)+1] = subs[i][j];
    }
    buffer[(j<<1)] = 0;
    n = (height - numsubs) + i;
    
    for (j=0; j<padding; j++)
    {
      txtimg[n][(j << 1) + indent] = 0x8F;
      txtimg[n][(j << 1) + 1 + indent] = ' ';
    }
    
    for (j=0; buffer[j] != 0; j++)
    {
      txtimg[n][j + (padding << 1) + indent] = buffer[j];
    }
    
    for (k=0; k<(longestsub-(j >> 1)); k++)
    {
      txtimg[n][(k << 1) + j + (padding << 1) + indent] = 0x8F;
      txtimg[n][(k << 1) + j + (padding << 1) + indent + 1] = ' ';
    }
    
    for (k=0; k<padding; k++)
    {
      txtimg[n][(k << 1) + (longestsub << 1) + (padding << 1) + indent] = 0x8F;
      txtimg[n][(k << 1) + 1 + (longestsub << 1) + (padding << 1) + indent] = ' ';
    }
    
    buffer[0] = 0;
  }
  
  printf("Subtitles overlayed!\n");
  
  return 1;
}

int writetxtimg(char *afn)
{
  if (!afn || !txtimg) return 0;
  FILE *afp = fopen(afn, "w");
  if (!afp)
  {
    fprintf(stderr, "Could not open file \"%s\"!\n", afn);
    return -1;
  }
  int i;
  for (i=0; txtimg[i] != NULL; i++)
  {
    fprintf(afp, "%s\r\n", txtimg[i]);
  }
  
  fclose(afp);
  return 1;
}

char *readsubs(char *afn)
{
  if (!afn) return NULL;
  FILE *afp = fopen(afn, "r");
  if (!afp)
  {
    fprintf(stderr, "Could not open \"%s\"!\n", afn);
    return NULL;
  }
  
  char *subtxt = (char *) malloc(sizeof(char)*MAXBUFFER);
  if (!subtxt)
  {
    fprintf(stderr, "Out of Memory!\n");
    return NULL;
  }
  
  int i, n = MAXBUFFER, a;
  for (i = 0; !feof(afp); i++)
  {
    if (i<n-1)
    {
      a = fgetc(afp);
      if (a != EOF) subtxt[i] = ((char) a);
    }
    else
    {
      n += MAXBUFFER;
      if (!realloc(subtxt, n))
      {
        free(subtxt);
        fprintf(stderr, "Out of Memory!\n");
        return NULL;
      }
    }
  }
  subtxt[i] = 0;
  return subtxt;
}


int main(int argc, char *argv[])
{
  char *subtxt = NULL;
  if (argc != 4)
  {
    printf("ImgTxt Subtitle Overlayer\n2019 DHeadshot's Software Creations\n");
    printf("Usage:\n  %s <InTxtImg> <OutTxtImg> <TextFile>\n", argv[0]);
    printf("Where:\n  <InTxtImg> is the TxtImg to work on.\n");
    printf("  <OutTxtImg> is the TxtImg to write to (can be the same as <InTxtImg>).\n");
    printf("  <TextFile> is a text file containg the subtitle text to overlay.\n");
    return 1;
  }
  
  int ret = loadtxtimg(argv[1]);
  if (ret != 1) return 2;
  gettxtimgstats();
  subtxt = readsubs(argv[3]);
  if (!subtxt)
  {
    freetxtimg();
    return 3;
  }
  ret = makesubs(subtxt);
  if (ret != 1)
  {
    freetxtimg();
    free(subtxt);
    return 4;
  }
  
#ifdef DEBUG
  printf("Subs:\n");
  for (ret=0; subs[ret] != NULL; ret++)
  {
    printf("%s\n", subs[ret]);
  }
  printf("(%d line(s))\n", ret);
#endif
  
  ret = addsubs();
  if (ret != 1)
  {
    freetxtimg();
    freesubs();
    free(subtxt);
    return 5;
  }
  ret = writetxtimg(argv[2]);
  if (ret != 1)
  {
    freetxtimg();
    freesubs();
    free(subtxt);
    return 6;
  }
  
  freesubs();
  freetxtimg();
  free(subtxt);
  return 0;
}
