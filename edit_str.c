/*
 * $Id: edit_str.c,v 1.3 1995/01/17 12:33:59 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: edit_str.c,v $
 * Revision 1.3  1995/01/17 12:33:59  sev
 * Now run screen is done
 * Revision 1.2  1995/01/14  15:08:09  sev Menu works
 * right. Compiler also. Revision 1.1  1995/01/07  20:05:08  sev Initial
 * revision
 * 
 * 
 */

#include <stdio.h>
#include "estruct.h"
#include "eproto.h"
#include "edef.h"

#define ENTER		CTRL|'M'
#define HOMEKEY		CTRL|'A'
#define PGUPKEY		SPEC|'Z'
#define LEFTKEY		SPEC|'B'
#define ENDKEY		CTRL|'E'
#define RIGHTKEY	SPEC|'F'
#define DELKEY		CTRL|'D'
#define BACKSPACE	CTRL|'H'
#define OVER		SPEC|'C'
#define HELP		SPEC|'1'
#define ESC		CTRL|'G'

void update_edit_str (int, int, int, int, int, char *, char);
void put_status (int, int, int, int, int);
void delete_char (char *, int);
void insert_char (char *, int, int);
void chmod (int, int, int);
void winopen (int, int, int, int, char *);
void winclose (int y1, int x1, int y2, int x2);

/**** Функция редактирования строки в окне ****/

/*
 * Функция позволяет редактировать в окне заданного размера строку
 * 
 * сдвигая ее влево иои вправо и помечая выход строки за рамки окна. Функция возвращает отредактированную строку
 * 
 */
int edit_string (int row, int start_col, int end_col, int buffersize,
		  char *originalstring, char *title)
{
  int ch;			  /* код символа */
  int endcolumn, width;

  /* начальная и конечная координаты строки на экране, ширина окна */
  int beg_status, end_status;

  /* флаги выхода строки за рамки окна */
  int length, rest;

  /* текущая длина строки, длина остатка строки справа от курсора */
  char *buffer;			  /* буфер для редактирования */
  int entry;			  /* номер вхождения в цикл */
  int cursorpos_w;		  /* позиция курсора в окне */
  char *pos_beg;		  /* начало видимой части текста */
  char *cursorpos_s;		  /* указатель на позицию курсора в строке */
  int ret_status;		  /* возвращаемое значение */
  int wdo;			  /* указатель на окно */
  int mode;			  /* текущий режим ( вставка, замена ) переключение ПТ */

  /* зарезервировать буфер для редактирования строки размера buffersize */
  if (!(buffer = (char *) malloc (buffersize + 5)))
    return 0;

  /* открыть окно */
  winopen (row - 1, start_col - 1, row + 1, end_col + 1, title);

  /* вычислить промежуточные значения */
  width = endcolumn = end_col - start_col;

  /* максимальная длина строки */
  buffersize--;

  /*
   * скопировать строку в буфер и получить ее длину, задать начальное значение остатка
   */
  memset (buffer, 0, buffersize + 5);
  length = rest = strlen (originalstring);
  strcpy (buffer, originalstring);

  /* начальное положение курсоров и нач. знач. всех индикаторов */
  cursorpos_s = pos_beg = buffer;
  cursorpos_w = 0;
  beg_status = 0;
  end_status = (rest <= width) ? 0 : 1;
  mode = 0;			  /* вставка */

  /* вывести строку и текущий режим на экран */
  update_edit_str (row, start_col, end_col, beg_status, end_status, pos_beg, 1);
  chmod (row, end_col, mode);

  /* установить курсор на нужную позицию */
  TTmove (row, cursorpos_w + start_col);
  TTflush ();

  entry = 0;			  /* нулевое вхождение в цикл */

key_loop:
  /* прочитать код с клавиатуры */
  switch (ch = getkey ())
  {
    case ESC:			  /* нажата клавиша АР2 */
      ret_status = 0;
      goto quit;		  /* перейтина возврат */
    case ENTER:		  /* нажата клавиша ВК */
      ret_status = 1;
      /* скопировать отредактированную строку в первоначальный буфер */
      strcpy (originalstring, buffer);
      goto quit;		  /* перейтина возврат */
    case HOMEKEY:		  /* нажата клавиша РТ */
      /* если нулевое вхождение, то установить атрибуты редактированной стрроки */
      if (!entry)
	entry = 1;
      else
      {
	if (rest != length)
	{
	  cursorpos_w = 0;
	  cursorpos_s = pos_beg = buffer;
	  rest = length;
	  beg_status = 0;
	  end_status = (rest <= width) ? 0 : 1;
	}
	else
	  goto key_loop;
      }
      update_edit_str (row, start_col, end_col, beg_status,
		       end_status, pos_beg, 0);
      break;
    case PGUPKEY:		  /* нажата клавиша УТ */
    case CTRL | 'Z':
      if (!entry)
	entry = 1;
      length = rest = strlen (originalstring);
      memset (buffer, 0, buffersize + 5);
      strcpy (buffer, originalstring);
      cursorpos_s = pos_beg = buffer;
      cursorpos_w = 0;
      beg_status = 0;
      end_status = (rest <= width) ? 0 : 1;
      update_edit_str (row, start_col, end_col, beg_status,
		       end_status, pos_beg, 0);
      break;
    case LEFTKEY:
    case CTRL | 'B':
      if (!entry)
	entry = 1;
      if (cursorpos_w > 0)
      {
	cursorpos_w--;
	cursorpos_s--;
	rest++;
	break;
      }
      else
      {
	if (rest < length)
	{
	  cursorpos_s--;
	  rest++;
	  pos_beg--;
	  beg_status = (rest == length) ? 0 : 1;
	  end_status = (rest <= width) ? 0 : 1;
	  update_edit_str (row, start_col, end_col, beg_status,
			   end_status, pos_beg, 0);
	}
	break;
      }
    case ENDKEY:		  /* нажата клавиша ВПС */
      if (!entry)
	entry = 1;
      if (rest)
      {
	cursorpos_s = buffer + length;
	end_status = 0;
	rest = 0;
	if (length < width)
	{
	  cursorpos_w = length;
	  pos_beg = buffer;
	  beg_status = 0;
	}
	else
	{
	  cursorpos_w = endcolumn;
	  pos_beg = buffer - width + length;
	  beg_status = 1;
	}
	update_edit_str (row, start_col, end_col, beg_status,
			 end_status, pos_beg, 0);
	break;
      }
      goto key_loop;
    case RIGHTKEY:
    case CTRL | 'F':
      if (!entry)
      {
	update_edit_str (row, start_col, end_col, beg_status,
			 end_status, pos_beg, 0);
	entry = 1;
      }
      if (cursorpos_w < endcolumn)
      {
	if (rest)
	{
	  cursorpos_w++;
	  cursorpos_s++;
	  rest--;
	  break;
	}
	goto key_loop;
      }
      else
      {
	if (rest)
	{
	  cursorpos_s++;
	  rest--;
	  pos_beg++;
	  beg_status = ((length - rest) < width) ? 0 : 1;
	  end_status = (rest <= 1) ? 0 : 1;
	  update_edit_str (row, start_col, end_col, beg_status,
			   end_status, pos_beg, 0);
	}
	break;
      }
    case DELKEY:		  /* нажата клавиша ВНТ */
      if (!entry)
	entry = 1;
      if (rest)
      {
	delete_char (cursorpos_s, rest);
	rest--;
	length--;
	if (rest < endcolumn - cursorpos_w)
	{
	  end_status = 0;
	  if (beg_status)
	  {
	    cursorpos_w++;
	    pos_beg--;
	    beg_status = (length - rest < cursorpos_w) ? 0 : 1;
	    update_edit_str (row, start_col, end_col, beg_status,
			     end_status, pos_beg, 0);
	    break;
	  }
	  update_edit_str (row, start_col, end_col, beg_status,
			   end_status, pos_beg, 0);
	}
	else
	{
	  end_status = (rest <= width - cursorpos_w) ? 0 : 1;
	  update_edit_str (row, start_col, end_col, beg_status,
			   end_status, pos_beg, 0);
	}
      }
      break;
    case BACKSPACE:		  /* нажата клавиша ВШ */
      if (!entry)
	entry = 1;
      if (rest < length)
      {
	cursorpos_s--;
	delete_char (cursorpos_s, rest + 1);
	length--;
	if (rest <= width - cursorpos_w)
	{
	  end_status = 0;
	  if (beg_status)
	  {
	    beg_status = (length - rest <= cursorpos_w) ? 0 : 1;
	    pos_beg--;
	    update_edit_str (row, start_col, end_col, beg_status,
			     end_status, pos_beg, 0);
	  }
	  else
	  {
	    cursorpos_w--;
	    update_edit_str (row, start_col, end_col, beg_status,
			     end_status, pos_beg, 0);
	    break;
	  }
	}
	else
	{
	  if (cursorpos_w > 0)
	  {
	    cursorpos_w--;
	    end_status = (rest <= width - cursorpos_w) ? 0 : 1;
	    update_edit_str (row, start_col, end_col, beg_status,
			     end_status, pos_beg, 0);
	    break;
	  }
	  else
	  {
	    if (rest == length)
	    {
	      beg_status = 0;
	      update_edit_str (row, start_col, end_col, beg_status,
			       end_status, pos_beg, 0);
	    }
	  }
	}
      }
      break;
    case OVER:			  /* нажата клавиша ПТ */
      if (mode)
	mode = 0;		  /* вставка */
      else
	mode = 1;		  /* замена */
      chmod (row, end_col, mode);
      break;
    case HELP:
      break;
    default:
      if (ch < 32 || ch > 255)
	goto key_loop;
      if (!entry)
      {
	memset (buffer, 0, buffersize + 5);
	length = rest = beg_status = end_status = cursorpos_w = 0;
	cursorpos_s = pos_beg = buffer;
	entry = 1;
	update_edit_str (row, start_col, end_col, beg_status,
			 end_status, pos_beg, 0);
	TTmove (row, start_col);
      }
      if ((!mode) || (!rest))
      {
	/* если буфер полон */
	if (length >= buffersize)
	  goto key_loop;
	insert_char (cursorpos_s, rest, ch);
	length++;
	if (cursorpos_w < endcolumn)
	{
	  end_status = (rest < width - cursorpos_w) ? 0 : 1;
	  update_edit_str (row, start_col, end_col, beg_status,
			   end_status, pos_beg, 0);
	  cursorpos_s++;
	  cursorpos_w++;
	  break;
	}
	pos_beg++;
	cursorpos_s++;
	beg_status = (length - rest < cursorpos_w - width) ? 0 : 1;
	update_edit_str (row, start_col, end_col, beg_status,
			 end_status, pos_beg, 0);
      }
      else
	/* режим замены */
      {
	cursorpos_s[0] = ch;
	if (cursorpos_w == width)
	  pos_beg++;
	else
	  cursorpos_w++;

	rest--;
	cursorpos_s++;
	update_edit_str (row, start_col, end_col, beg_status,
			 end_status, pos_beg, 0);
      }
  }
  TTmove (row, cursorpos_w + start_col);
  TTflush ();
  goto key_loop;

quit:
  free (buffer);
  winclose (row - 1, start_col - 1, row + 1, end_col + 1);
  if (!*originalstring)
    ret_status = 0;
  return ret_status;
}

void update_edit_str (int row, int startcolumn, int endcolumn, int beg_status,
		       int end_status, char *pos_beg, char attr)
{
  register int counter;

  TTmove (row, startcolumn);
  if (attr)
    TTrev (TRUE);
  for (counter = 0; counter <= (endcolumn - startcolumn) && pos_beg[counter];
       counter++)
    TTputc (pos_beg[counter]);
  for (; counter <= (endcolumn - startcolumn); counter++)
    TTputc (' ');

  if (attr)
    TTrev (FALSE);
  put_status (row, startcolumn, endcolumn, beg_status, end_status);
}

void put_status (int row, int startcolumn, int endcolumn, int beg_status,
		  int end_status)
{
  TTmove (row, startcolumn - 1);

  if (beg_status)
    TTputc ('<');
  else
    TTputc ('\272');

  TTmove (row, endcolumn + 1);

  if (end_status)
    TTputc ('>');
  else
    TTputc ('\272');
  TTflush ();
}

void delete_char (char *sourcestring, int stringlength)
{
  register int counter;

  for (counter = 1; counter <= stringlength; counter++)
    sourcestring[counter - 1] = sourcestring[counter];
}

void insert_char (char *sourcestring, int stringlength, int character)
{
  register int counter;

  for (counter = stringlength + 1; counter >= 0; counter--)
    sourcestring[counter] = sourcestring[counter - 1];
  sourcestring[0] = character;
}

void chmod (int row, int end_col, int mode)
{
  TTmove (row - 1, end_col - 7);

  if (mode)
    TTputs (" Ovr ");
  else
    TTputs (" Ins ");
  TTflush ();
}

/*
 * "ESC    - Выход без поиска.               ", "PgUp   - Вывод
 *           ", "Home   - В начало строки.                ", "End    -
 *                         ", "Insert - Переключение режима Ins/Ovr     ",
 * "BkSp   - Удаление символа перед курсором.", "Del    - Удаление символа
 *   ",
 */

void winopen (int y1, int x1, int y2, int x2, char *title)
{
  int i, j, c;

  c = (x2 - x1 - strlen (title)) / 2;

  TTmove (y1, x1);
  TTputc ('╔');
  for (i = 0; i < c; i++)
    TTputc ('═');
  TTputs (title);
  i += strlen (title) + 1;
  for (; i < x2 - x1; i++)
    TTputc ('═');
  TTputc ('╗');

  for (i = 1; i < y2 - y1 - 1; i++)
  {
    TTmove (i + y1 + 1, x1);
    TTputc ('║');
    for (j = 2; j < x2 - x1; j++)
      TTputc (' ');
    TTputc ('║');
  }
  TTmove (y2, x1);
  TTputc ('╚');
  for (i = 1; i < x2 - x1; i++)
    TTputc ('═');
  TTputc ('╝');
}

void winclose (int y1, int x1, int y2, int x2)
{
  int i;

  for (i = y1; i <= y2; i++)
    updoneline (i, x1, x2);
  TTmove (0, 0);
  update (TRUE);
}
