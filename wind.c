#include <stdio.h>
#include <graphics.h>

typedef struct WPTR
{
  int x, y;
  int x1, y1;
  char *image;
  char **text;
} WPTR;

void setgrmode(void);
void outmsg(int, int, char *);
void wopen(int, int, int, int, char **, WPTR *);
void wclose(WPTR *);
void selbar(int, WPTR *);
void unselbar(int, WPTR *);

struct textsettingstype textsets;

char *text1[5] =
{
	"Open",
	"New",
	"Save",
	"save As...",
	"Quit"
};

main()
{
  WPTR w1, w2;
  int key;
  int x;

  setgrmode();

  setcolor(WHITE);
  wopen(10, 10, 24, 16, text1, &w1);
  wopen(12, 12, 26, 18, w1.text, &w2);

  x = 0;
  selbar(0, &w2);
  while((key = getch()) != 27)
  {
	if(!key)
	{
	  key = getch();
	  switch(key)
	  {
		case 0x50:
		   if(x < 4)
		   {
			 unselbar(x, &w2);
			 x++;
			 selbar(x, &w2);
		   }
		   break;
		case 0x48:
		   if(x)
		   {
			 unselbar(x, &w2);
			 x--;
			 selbar(x, &w2);
		   }
		   break;
	  }
	}
  }

  wclose(&w2);
  getch();
  closegraph();
  return 0;
}

void setgrmode(void)
{
  int driver = EGA;
  int mode = EGAHI;

  initgraph(&driver, &mode, "");
  gettextsettings(&textsets);
  textsets.charsize *= 8;
}

void wopen(x, y, x1, y1, text, w)
int x, y, x1, y1;
char **text;
WPTR *w;
{
  int i, j;
  char buf[256];

  w->image = (char *)malloc(imagesize(textsets.charsize*x, textsets.charsize*y,
			textsets.charsize*(x1+1), textsets.charsize*(y1+1)));
  getimage(textsets.charsize*x, textsets.charsize*y,
			textsets.charsize*(x1+1), textsets.charsize*(y1+1), w->image);
  w->text = (char **)malloc(sizeof(char *)*(y1-y-2));
  for(i = 0; i < y1-y-1; i++)
  {
	w->text[i] = (char *)malloc(x1-x-2);
	strcpy(w->text[i], text[i]);
  }
  w->x = x;
  w->y = y;
  w->x1 = x1;
  w->y1 = y1;

  setfillstyle(SOLID_FILL, BLACK);
  bar(textsets.charsize*x, textsets.charsize*y,
			textsets.charsize*(x1+1), textsets.charsize*(y1+1));
  buf[0] = '╔';
  for(i = 1; i < x1-x-1; i++)
	buf[i] = '═';
  buf[i++] = '╗';
  buf[i] = 0;
  outmsg(x, y, buf);


  for(j = y+1; j < y1; j++)
  {
	buf[0] = '║';
	buf[1] = ' ';
	buf[2] = 0;
	strcat(buf, text[j-y-1]);
	for(i = strlen(buf); i < x1-x-1; i++)
	  buf[i] = ' ';
	buf[i++] = '║';
	buf[i] = 0;
	outmsg(x, j, buf);
  }

  buf[0] = '╚';
  for(i = 1; i < x1-x-1; i++)
	buf[i] = '═';
  buf[i++] = '╝';
  outmsg(x, y1, buf);

}

void outmsg(x, y, s)
int x, y;
char *s;
{
  outtextxy(textsets.charsize*x, textsets.charsize*y, s);
}

void wclose(w)
WPTR *w;
{
  int i;

  putimage(w->x*textsets.charsize, w->y*textsets.charsize, w->image, COPY_PUT);

  free(w->image);
  w->image = (char *)NULL;
  for(i = 0; i < w->y1-w->y-1; i++)
	free(w->text[i]);
  free(w->text);
  w->text = (char **)NULL;
}

void selbar(x, w)
int x;
WPTR *w;
{
  setfillstyle(SOLID_FILL, WHITE);
  bar((w->x+1)*textsets.charsize, (w->y+x+1)*textsets.charsize,
				(w->x1-1)*textsets.charsize, (w->y+x+2)*textsets.charsize);
  setcolor(BLACK);

  outmsg(w->x+2, w->y+x+1, w->text[x]);
}

void unselbar(x, w)
int x;
WPTR *w;
{
  setfillstyle(SOLID_FILL, BLACK);
  bar((w->x+1)*textsets.charsize, (w->y+x+1)*textsets.charsize,
				(w->x1-1)*textsets.charsize, (w->y+x+2)*textsets.charsize);
  setcolor(WHITE);

  outmsg(w->x+2, w->y+x+1, w->text[x]);
}

