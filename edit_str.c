/*
 * $Id: edit_str.c,v 1.1 1995/01/07 20:05:08 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: edit_str.c,v $
 * Revision 1.1  1995/01/07 20:05:08  sev
 * Initial revision
 *
 * 
 */

#include "estruct.h"
#include "etype.h"

#define ENTER		CTRL|'M'  /* ВК */
#define HOMEKEY		SPEC|'<'  /* РТ */
#define PGUPKEY		SPEC|'Z'  /* УТ */
#define LEFTKEY		SPEC|'B'
#define ENDKEY		SPEC|'>'  /* ВПС */
#define RIGHTKEY	SPEC|'F'
#define DELKEY		SPEC|'D'  /* ВНТ */
#define BACKSPACE	CTRL|'H'  /* ВШ */
#define OVER		SPEC|'C'  /* ПТ */
#define HELP		SPEC|'1'

void update_edit_str (int, int, int, int, int, char *, char);
void put_status (int, int, int, int, int);
void delete_char (char *, int);
void insert_char (char *, int, int);
void chmod (int, int, int);

/**** Функция редактирования строки в окне ****/

/*
 * Функция позволяет редактировать в окне заданного размера строку
 * 
 * 
 * сдвигая ее влево иои вправо и помечая выход строки за рамки окна. Функция возвращает отредактированную строку в том в
 *  row             - номер строки окна start_col       - левая координата
 * nd_col         - правая координата окна buffersize      - размер байт;
 * originalstring  - указатель на title           - указатель на sourceattr
 * - атрибуты для destattr        - атрибуты Выход: Функция , если строка не
 *  была нажата APE ), либо 1, если строка была отредактирована
 */
int edit_string (int row, int start_col, int nd_col, int buffersize,
	  char *originalstring, char *title, char sourceattr, char destattr)
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
  if ((wdo = wxopen (row - 1, start_col - 1, row + 1, end_col + 1, title,
	     BORDER + BD1 + ACTIVE + CURSOR + NOADJ + CURTAIN, 0, 0)) == -1)
    return 0;

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
  init_edit_string_help ();

  /* вывести строку и текущий режим на экран */
  update_edit_str (row, start_col, end_col, beg_status, end_status, pos_beg,
	  sourceattr);
  chmod (row, end_col, mode);

  /* установить курсор на нужную позицию */
  at (0, cursorpos_w);

  entry = 0;			  /* нулевое вхождение в цикл */

key_loop:
  /* прочитать код с клавиатуры */
  switch (ch = getone ())
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
	      end_status, pos_beg, destattr);
      break;
    case PGUPKEY:		  /* нажата клавиша УТ */
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
	      end_status, pos_beg, destattr);
      break;
    case LEFTKEY:
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
		  end_status, pos_beg, destattr);
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
		end_status, pos_beg, destattr);
	break;
      }
      goto key_loop;
    case RIGHTKEY:
      if (!entry)
      {
	update_edit_str (row, start_col, end_col, beg_status,
		end_status, pos_beg, destattr);
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
		  end_status, pos_beg, destattr);
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
		    end_status, pos_beg, destattr);
	    break;
	  }
	  update_edit_str (row, start_col, end_col, beg_status,
		  end_status, pos_beg, destattr);
	}
	else
	{
	  end_status = (rest <= width - cursorpos_w) ? 0 : 1;
	  update_edit_str (row, start_col, end_col, beg_status,
		  end_status, pos_beg, destattr);
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
		    end_status, pos_beg, destattr);
	  }
	  else
	  {
	    cursorpos_w--;
	    update_edit_str (row, start_col, end_col, beg_status,
		    end_status, pos_beg, destattr);
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
		    end_status, pos_beg, destattr);
	    break;
	  }
	  else
	  {
	    if (rest == length)
	    {
	      beg_status = 0;
	      update_edit_str (row, start_col, end_col, beg_status,
		      end_status, pos_beg, destattr);
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
      edit_string_help_msg ();
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
		end_status, pos_beg, destattr);
	at (0, 0);
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
		  end_status, pos_beg, destattr);
	  cursorpos_s++;
	  cursorpos_w++;
	  break;
	}
	pos_beg++;
	cursorpos_s++;
	beg_status = (length - rest < cursorpos_w - width) ? 0 : 1;
	update_edit_str (row, start_col, end_col, beg_status,
		end_status, pos_beg, destattr);
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
		end_status, pos_beg, destattr);
      }
  }
  at (0, cursorpos_w);
  goto key_loop;

quit:
  free (buffer);
  wclose (wdo);
  if (!*originalstring)
    ret_status = 0;
  return ret_status;
}

void update_edit_str (int row, int startcolumn, int endcolumn, int beg_status,
	      int end_status, char *pos_beg, char attr)
{
  register int counter;

  at (0, 0);
  for (counter = 0; counter <= (endcolumn - startcolumn); counter++)
    vcputc (pos_beg[counter], attr);

  put_status (row, startcolumn, endcolumn, beg_status, end_status);
}

void put_status (int row, int startcolumn, int endcolumn, int beg_status,
		  int end_status)
{
  /* отключение оконной логики */
  override = 1;

  at (row, startcolumn - 1);

  if (beg_status)
    vcputc ('<', vc.white + vc.bg * vc.black);
  else
    vcputc ('\272', vc.white + vc.bg * vc.black);

  at (row, endcolumn + 1);

  if (end_status)
    vcputc ('>', vc.white + vc.bg * vc.black);
  else
    vcputc ('\272', vc.white + vc.bg * vc.black);

  /* включение оконной логики */
  override = 0;
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
  /* отключение оконной логики */
  override = 1;

  at (row - 1, end_col - 7);

  if (mode)
    vcputs (" Ovr ", vc.white + vc.bg * vc.black);
  else
    vcputs (" Ins ", vc.white + vc.bg * vc.black);

  /* включение оконной логики */
  override = 0;
}

void edit_string_help_msg ()	  /* функция вызывается по F1 */
{
  int edit_string_help_w;	  /* окно помощи			 */
  int i;
  static char *edit_string_help_window[] =
  {
    "ESC    - Выход без поиска.               ",
    "PgUp   - Вывод исходной строки.          ",
    "Home   - В начало строки.                ",
    "End    - В конец.                        ",
    "Insert - Переключение режима Ins/Ovr     ",
    "BkSp   - Удаление символа перед курсором.",
    "Del    - Удаление символа над курсором.  ",
  };


  edit_string_help_w = wxopen (10 - sizeof (edit_string_help_sm_wind) / sizeof (char *) / 2,
			       35 - strlen (edit_string_help_window[0]) /2,
	       15 + sizeof (edit_string_help_sm_wind) / sizeof (char *) / 2,
			       45 + strlen (edit_string_help_window[0]) /2,
			   " Помощь ", ACTIVE + BORDER + BD1 + NOADJ, 0, 0);
  for (i = 0; i < sizeof (edit_string_help_sm_wind) / sizeof (char *); i++)
    atsay (i + 1, 5, edit_string_help_window[i]);
  i += 2;
  atsay (i, 9, "Нажмите любую клавишу");
  getone ();
  wclose (edit_string_help_w);
}
