#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

/*** Includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>

/*** Defines
 * multi line comment hahahaha
 * this is so stupid ***/
#define CTRL_KEY(x) ((x) & 0x1f)
#define ABUF_INIT {NULL, 0}
#define KAKU_VERSION "0.0.2"
#define KAKU_TAB_STOP 8
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)
/*** Data ***/
struct editorSyntax
{
  char *filetype;
  char **filematch;
  char **keyword;
  char *singleline_comment_start;
  char *multiline_comment_start;
  char *multiline_comment_end;
  int flags;
};

typedef struct erow
{
  int idx;
  char *chars;
  char *render;
  int rsize;
  int size;
  unsigned char *hl;
  int hl_open_comment;
} erow;

struct editorConfig
{
  int cx;
  int cy;
  int rx;
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  int numrows;
  int dirty;
  int prompt;
  char statusmsg[80];
  time_t statusmsg_time;
  char *filename;
  erow *row;
  struct editorSyntax *syntax;
  struct termios orig_termios;
} EConfig;

enum editorKey
{
  Backspace = 127,
  MoveLeft = 1000,
  MoveRight,
  MoveUp,
  MoveDown,
  MoveEndOfLine,
  MoveStartOfLine,
  PageUp,
  PageDown,
  Delete
};

enum editorHighlight
{
  HL_NORMAL = 0,
  HL_STRING,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_NUMBER,
  HL_MATCH
};

/*** Filetype ***/
char *C_HL_extensions[] = {".c", ".cpp", ".h", NULL};
char *C_HL_keywords[] = {"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union",
                         "typedef", "static", "enum", "class", "case", "int|", "long|", "double|", "float|", "char|", "unsigned|",
                         "signed|", "void|", NULL};

struct editorSyntax HLDB[] = {
    {"c",
     C_HL_extensions,
     C_HL_keywords,
     "//", "/*", "*/",
     HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/*** Forware Declaration ***/
char *editorPrompt(char *prompt, void (*callback)(char *, int));
int editorSyntaxToColor(int);

/*** Append Buffer ***/
struct abuf
{
  char *b;
  int len;
};

void abAppend(struct abuf *ab, const char *s, const int len)
{
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL)
    return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}

void abFree(struct abuf *ab)
{
  free(ab->b);
}

/*** Output  ***/
int editorRowCxToRx(erow *row, int cx)
{
  int rx = 0;
  for (int j = 0; j < cx; j++)
  {
    if (row->chars[j] == '\t')
      rx += (KAKU_TAB_STOP - 1) - (rx % KAKU_TAB_STOP);
    rx++;
  }
  return rx;
}

int editorRowRxToCx(erow *row, int rx)
{
  int cur_rx = 0;
  int cx;
  for (cx = 0; cx < row->size; cx++)
  {
    if (row->chars[cx] == '\t')
      cur_rx += (KAKU_TAB_STOP - 1) - (cur_rx % KAKU_TAB_STOP);
    cur_rx++;
    if (cur_rx > rx)
      return cx;
  }
  return cx;
}

void editorScroll()
{
  EConfig.rx = 0;
  if (EConfig.cy < EConfig.numrows)
    EConfig.rx = editorRowCxToRx(&EConfig.row[EConfig.cy], EConfig.cx);

  if (EConfig.cy < EConfig.rowoff)
    EConfig.rowoff = EConfig.cy;
  if (EConfig.cy >= EConfig.rowoff + EConfig.screenrows)
    EConfig.rowoff = EConfig.cy - EConfig.screenrows + 1;
  if (EConfig.rx < EConfig.coloff)
    EConfig.coloff = EConfig.cy;
  if (EConfig.rx >= EConfig.coloff + EConfig.screencols)
    EConfig.coloff = EConfig.rx - EConfig.screencols + 1;
}

void editorDrawRows(struct abuf *ab)
{
  for (int y = 0; y < EConfig.screenrows; y++)
  {
    int filerow = y + EConfig.rowoff;
    if (filerow >= EConfig.numrows)
    {
      if (EConfig.numrows == 0 && y == EConfig.screenrows / 3)
      {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome), "FaT editor -- version %s", KAKU_VERSION);
        if (welcomelen > EConfig.screencols)
          welcomelen = EConfig.screencols;
        int padding = (EConfig.screencols - welcomelen) / 2;
        if (padding)
        {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--)
          abAppend(ab, " ", 1);

        abAppend(ab, welcome, welcomelen);
      }
      else
      {
        abAppend(ab, "~", 1);
      }
    }
    else
    {
      int len = EConfig.row[filerow].rsize - EConfig.coloff;
      if (len < 0)
        len = 0;
      if (len > EConfig.screencols)
        len = EConfig.screencols;

      char *c = &EConfig.row[filerow].render[EConfig.coloff];
      unsigned char *hl = &EConfig.row[filerow].hl[EConfig.coloff];
      int current_color = -1;
      for (int i = 0; i < len; i++)
      {
        if (iscntrl(c[i]))
        {
          char sym = (c[i] <= 26) ? '@' + c[i] : '?';
          abAppend(ab, "\x1b[7m", 4);
          abAppend(ab, &sym, 1);
          abAppend(ab, "\x1b[m", 3);
          if (current_color != -1)
          {
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
            abAppend(ab, buf, clen);
          }
        }
        else if (current_color == -1)
        {
          int color = editorSyntaxToColor(hl[i]);
          char buf[16];
          int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
          abAppend(ab, buf, clen);
        }
        abAppend(ab, &c[i], 1);
      }
      current_color = -1;
    }
    abAppend(ab, "\x1b[K", 3);
    abAppend(ab, "\r\n", 2);
  }
}

void editorDrawStatusBar(struct abuf *ab)
{
  abAppend(ab, "\x1b[7m", 4);
  char status[80];
  char rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                     EConfig.filename ? EConfig.filename : "[No Name]", EConfig.numrows,
                     EConfig.dirty ? "(modified)" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", EConfig.syntax ? EConfig.syntax->filetype : "No filetype", EConfig.cy + 1, EConfig.numrows);
  if (len > EConfig.screencols)
    len = EConfig.screencols;
  abAppend(ab, status, len);
  while (len < EConfig.screencols)
  {
    if (EConfig.screencols - len == rlen)
    {
      abAppend(ab, rstatus, rlen);
      break;
    }
    else
    {
      abAppend(ab, " ", 1);
      len++;
    }
  }
  abAppend(ab, "\x1b[m", 3);
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab)
{
  abAppend(ab, "\x1b[K", 3);
  int msglen = strlen(EConfig.statusmsg);
  if (msglen > EConfig.screencols)
    msglen = EConfig.screencols;
  if (msglen && time(NULL) - EConfig.statusmsg_time < 5)
    abAppend(ab, EConfig.statusmsg, msglen);
}

void editorRefreshScreen()
{
  editorScroll();

  struct abuf ab = ABUF_INIT;
  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (EConfig.cy - EConfig.rowoff) + 1, (EConfig.rx - EConfig.coloff) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(EConfig.statusmsg, sizeof(EConfig.statusmsg), fmt, ap);
  va_end(ap);
  EConfig.statusmsg_time = time(NULL);
}

/*** Terminal ***/
void die(const char *s)
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  perror(s);
  exit(1);
}

void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &EConfig.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &EConfig.orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = EConfig.orig_termios;

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

int editorReadKey()
{
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
  {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  if (c == '\x1b')
  {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';
    if (seq[0] == '[')
    {
      if (seq[1] >= '0' && seq[1] <= '9')
      {
        if (read(STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';
        if (seq[2] == '~')
        {
          switch (seq[1])
          {
          // home key
          case '1':
            break;
            // Delete
          case '3':
            return Delete;
          // end key
          case '4':
            break;
            // page up
          case '5':
            break;
            // page down
          case '6':
            break;
            // home key
          case '7':
            break;
            // end key
          case '8':
            break;
          }
        }
      }
      else
      {
        switch (seq[1])
        {
          // up arrow
        case 'A':
          break;
          // down arrow
        case 'B':
          break;
          // right arrow
        case 'C':
          break;
          // left arrow
        case 'D':
          break;
          // home key
        case 'H':
          break;
          // end key
        case 'F':
          break;
        }
      }
    }
    else if (seq[0] == 'O')
    {
      switch (seq[1])
      {
      // home key
      case 'H':
        break;
      // end key
      case 'F':
        break;
      }
    }
  }
  switch (c)
  {
  case 'w':
    return MoveUp;
  case 's':
    return MoveDown;
  case 'e':
    return MoveRight;
  case 'a':
    return MoveLeft;
  }
  // }
  //     return '\x1b';
  // }
  // else
  // {

  return c;
  // }
}

int getCursorPosition(int *rows, int *cols)
{
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\xb[6n", 4) != 5)
    return -1;

  while (i < sizeof(buf) - 1)
  {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
    return -1;

  editorReadKey();
  return -1;
}

int getWindowSize(int *rows, int *cols)
{
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
  {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    return getCursorPosition(rows, cols);
  }
  else
  {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** Syntax Highlighting ***/
int is_separator(int c)
{
  return isspace(c) || c == '\0' || strchr(",.()+=-/*~%<>[];", c) != NULL;
}

void editorUpdateSyntax(erow *row)
{
  row->hl = realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);

  if (!EConfig.syntax)
    return;

  char **keyword = EConfig.syntax->keyword;

  char *scs = EConfig.syntax->singleline_comment_start;
  char *mcs = EConfig.syntax->multiline_comment_start;
  char *mce = EConfig.syntax->multiline_comment_end;

  int scs_len = scs ? strlen(scs) : 0;
  int mcs_len = mcs ? strlen(mcs) : 0;
  int mce_len = mce ? strlen(mce) : 0;

  int prev_sep = 1;
  int in_string = 0;
  int in_comment = (row->idx > 0 && EConfig.row[row->idx - 1].hl_open_comment);

  int i = 0;
  while (i < row->rsize)
  {
    char c = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

    if (scs_len && !in_string && !in_comment && !strncmp(&row->render[i], scs, scs_len))
    {
      memset(&row->hl[i], HL_COMMENT, row->rsize - i);
      break;
    }

    if (mcs_len && mce_len && !in_string)
    {
      if (in_comment)
      {
        row->hl[i] = HL_MLCOMMENT;
        if (!strncmp(&row->render[i], mce, mce_len))
        {
          memset(&row->hl[i], HL_MLCOMMENT, mce_len);
          i += mce_len;
          in_comment = 0;
          prev_sep = 1;
          continue;
        }
        else
        {
          i++;
          continue;
        }
      }
      else if (!strncmp(&row->render[i], mcs, mcs_len))
      {
        memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
        i += mcs_len;
        in_comment = 1;
        continue;
      }
    }

    if (EConfig.syntax->flags & HL_HIGHLIGHT_NUMBERS)
    {
      if (in_string)
      {
        row->hl[i] = HL_STRING;
        if (c == '\\' && i + 1 < row->rsize)
        {
          row->hl[i + 1] = HL_STRING;
          i += 2;
          continue;
        }
        if (c == in_string)
          in_string = 0;
        i++;
        prev_sep = 1;
        continue;
      }
      else
      {
        if (c == '"' || c == '\'')
        {
          in_string = c;
          row->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }
    if (EConfig.syntax->flags & HL_HIGHLIGHT_NUMBERS)
    {
      if (isdigit(c) && (prev_sep || prev_hl == HL_NUMBER) || (c == '.' && prev_hl == HL_NUMBER))
      {
        row->hl[i] = HL_NUMBER;
        prev_sep = 0;
        i++;
        continue;
      }
    }
    if (prev_sep)
    {
      for (int j = 0; keyword[j]; j++)
      {
        int klen = strlen(keyword[j]);
        int kw2 = keyword[j][klen - 1] == '|';
        if (kw2)
          klen--;
        if (!strncmp(&row->render[i], keyword[j], klen) && is_separator(row->render[i + klen]))
        {
          memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          i += klen;
          break;
        }
      }
      if (keyword[i])
      {
        prev_sep = 0;
        continue;
      }
    }
    i++;
    prev_sep = is_separator(c);
  }
  int changed = (row->hl_open_comment != in_comment);
  row->hl_open_comment = in_comment;
  if (changed && row->idx + 1 < EConfig.numrows)
    editorUpdateSyntax(&EConfig.row[row->idx + 1]);
}

int editorSyntaxToColor(int hl)
{
  switch (hl)
  {
  case HL_COMMENT:
  case HL_MLCOMMENT:
    return 36;
  case HL_KEYWORD1:
    return 33;
  case HL_KEYWORD2:
    return 32;
  case HL_STRING:
    return 35;
  case HL_NUMBER:
    return 31;
  case HL_MATCH:
    return 34;
  default:
    return 39;
  }
}

void editorSelectSyntaxHighlight()
{
  EConfig.syntax = NULL;
  if (!EConfig.filename)
    return;

  char *ext = strrchr(EConfig.filename, '.');

  for (unsigned int i = 0; i < HLDB_ENTRIES; i++)
  {
    struct editorSyntax *s = &HLDB[i];
    int j = 0;
    while (s->filematch[j])
    {
      int is_ext = (s->filematch[j][0] == '.');
      if ((is_ext && ext && !strcmp(ext, s->filematch[j])) || (!is_ext && strstr(EConfig.filename, s->filematch[j])))
      {
        EConfig.syntax = s;

        for (int i = 0; i < EConfig.numrows; i++)
        {
          editorUpdateSyntax(&EConfig.row[i]);
        }

        return;
      }
      j++;
    }
  }
}

/** Row Operation ***/
void editorUpdateRow(erow *row)
{
  int tabs = 0;
  for (int j = 0; j < row->size; j++)
    if (row->chars[j] == '\t')
      tabs++;
  free(row->render);
  row->render = malloc(row->size + tabs * (KAKU_TAB_STOP - 1) + 1);

  int idx = 0;
  for (int j = 0; j < row->size; j++)
  {
    if (row->chars[j] == '\t')
    {
      row->render[idx++] = ' ';
      while (idx % KAKU_TAB_STOP != 0)
        row->render[idx++] = ' ';
    }
    else
    {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
  editorUpdateSyntax(row);
}

void editorInsertRow(int at, char *s, size_t len)
{
  if (at < 0 || at > EConfig.numrows)
    return;

  EConfig.row = realloc(EConfig.row, sizeof(erow) * (EConfig.numrows + 1));
  memmove(&EConfig.row[at + 1], &EConfig.row[at], sizeof(erow) * (EConfig.numrows - at));
  for (int i = at + 1; i <= EConfig.numrows; i++)
  {
    EConfig.row[i].idx++;
  }

  EConfig.row[at].idx = at;
  EConfig.row[at].hl_open_comment = 0;

  EConfig.row[at].size = len;
  EConfig.row[at].chars = malloc(len + 1);
  memcpy(EConfig.row[at].chars, s, len);
  EConfig.row[at].chars[len] = '\0';

  EConfig.row[at].rsize = 0;
  EConfig.row[at].render = NULL;
  EConfig.row[at].hl = NULL;
  editorUpdateRow(&EConfig.row[at]);
  EConfig.numrows++;
  EConfig.dirty++;
}

void editorFreeRow(erow *row)
{
  free(row->render);
  free(row->chars);
  free(row->hl);
}

void editorDelRow(int at)
{
  if (at < 0 || at > EConfig.numrows)
    return;

  editorFreeRow(&EConfig.row[at]);
  memmove(&EConfig.row[at], &EConfig.row[at + 1], sizeof(erow) * (EConfig.numrows - at - 1));
  for (int i = at; i <= EConfig.numrows - 1; i++)
  {
    EConfig.row[i].idx--;
  }
  EConfig.numrows--;
  EConfig.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c)
{
  if (at < 0 || at > row->size)
    at = row->size;
  row->chars = realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  EConfig.dirty++;
}

void editorRowDelChar(erow *row, int at)
{
  if (at < 0 || at >= row->size)
    return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  EConfig.dirty++;
}

void editorInsertChar(int c)
{
  if (EConfig.cy == EConfig.numrows)
  {
    editorInsertRow(EConfig.numrows, "", 0);
  }
  editorRowInsertChar(&EConfig.row[EConfig.cy], EConfig.cx, c);
  EConfig.cx++;
}

void editorInsertNewline()
{
  if (EConfig.cx == 0)
    editorInsertRow(EConfig.cy, "", 0);
  else
  {
    erow *row = &EConfig.row[EConfig.cy];
    editorInsertRow(EConfig.cy + 1, &row->chars[EConfig.cx], row->size - EConfig.cx);
    row = &EConfig.row[EConfig.cy];
    row->size = EConfig.cx;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
  }
  EConfig.cy++;
  EConfig.cx = 0;
}

void editorRowAppendString(erow *row, char *s, size_t len)
{
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  EConfig.dirty++;
}

void editorDelChar()
{
  if ((EConfig.cx == 0 && EConfig.cy == 0) || EConfig.cy == EConfig.numrows)
    return;

  erow *row = &EConfig.row[EConfig.cy];
  if (EConfig.cx > 0)
  {
    editorRowDelChar(row, EConfig.cx - 1);
    EConfig.cx--;
  }
  else
  {
    EConfig.cx = EConfig.row[EConfig.cy - 1].size;
    editorRowAppendString(&EConfig.row[EConfig.cy - 1], row->chars, row->size);
    editorDelRow(EConfig.cy);
    EConfig.cy--;
  }
}

/*** File I/O ***/
char *editorRowsToString(int *buflen)
{
  int totlen = 0;
  for (int j = 0; j < EConfig.numrows; j++)
  {
    totlen += EConfig.row[j].size + 1;
  }
  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (int j = 0; j < EConfig.numrows; j++)
  {
    memcpy(p, EConfig.row[j].chars, EConfig.row[j].size);
    p += EConfig.row[j].size;
    *p = '\n';
    p++;
  }
  return buf;
}

void editorOpen(const char *filename)
{
  free(EConfig.filename);
  EConfig.filename = strdup(filename);

  editorSelectSyntaxHighlight();

  FILE *fp = fopen(filename, "r");
  if (!fp)
    die("open");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while ((linelen = getline(&line, &linecap, fp)) != -1)
  {
    while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      linelen--;
    editorInsertRow(EConfig.numrows, line, linelen);
  }
  free(line);
  fclose(fp);
  EConfig.dirty = 0;
}

void editorSave()
{
  if (EConfig.filename == NULL)
  {
    EConfig.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
    if (EConfig.filename == NULL)
    {
      editorSetStatusMessage("Save Aborted!");
      return;
    }
    editorSelectSyntaxHighlight();
  }

  int len;
  char *buf = editorRowsToString(&len);

  int fd = open(EConfig.filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1)
  {
    if (ftruncate(fd, len) != -1 && write(fd, buf, len) == len)
    {
      close(fd);
      free(buf);
      editorSetStatusMessage("%d bytes written to disk", len);
      return;
    }
    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
  }

  EConfig.dirty = 0;
}

void editorClose()
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  exit(0);
}

/*** Find ***/
void editorFindCallback(char *query, int key)
{
  static int last_match = -1;
  static int direction = 1;

  static int saved_hl_line;
  static char *saved_hl = NULL;

  if (saved_hl)
  {
    memcpy(EConfig.row[saved_hl_line].hl, saved_hl, saved_hl_line);
    saved_hl = NULL;
    free(saved_hl);
  }

  if (key == '\r' || key == '\x1b')
  {
    last_match = -1;
    direction = 1;
    return;
  }
  else if (key == MoveRight || key == MoveDown)
  {
    direction = 1;
  }
  else if (key == MoveLeft || key == MoveUp)
  {
    direction = -1;
  }
  else
  {
    last_match = -1;
    direction = 1;
  }

  if (last_match == -1)
    direction = 1;
  int current = last_match;

  for (int i = 0; i < EConfig.numrows; i++)
  {
    current += direction;
    if (current == -1)
      current = EConfig.numrows - 1;

    erow *row = &EConfig.row[current];
    char *match = strstr(row->render, query);
    if (match)
    {
      last_match = current;
      EConfig.cy = current;
      EConfig.cx = editorRowRxToCx(row, match - row->render);
      EConfig.rowoff = EConfig.numrows;

      saved_hl = malloc(row->rsize);
      saved_hl_line = current;
      memcpy(saved_hl, row->hl, row->rsize);
      memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
      break;
    }
  }
}

void editorFind()
{
  int saved_cx = EConfig.cx;
  int saved_cy = EConfig.cy;
  int saved_coloff = EConfig.coloff;
  int saved_rowoff = EConfig.rowoff;
  char *query = editorPrompt("Search: %s (ESC/Arrow/Enter)", editorFindCallback);
  if (query == NULL)
  {
    EConfig.cx = saved_cx;
    EConfig.cy = saved_cy;
    EConfig.rowoff = saved_rowoff;
    EConfig.coloff = saved_coloff;
  }
  else
    free(query);
}

/*** Input  ***/
char *editorPrompt(char *prompt, void (*callback)(char *, int))
{
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';
  while (1)
  {
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();

    int c = editorReadKey();
    switch (c)
    {
    case Backspace:
    case CTRL_KEY('h'):
    case Delete:
      if (buflen != 0)
        buf[--buflen] == '\0';
      break;
    case '\x1b':
      editorSetStatusMessage("");
      if (callback)
        callback(buf, c);
      free(buf);
      return NULL;
    case '\r':
      if (buflen != 0)
      {
        editorSetStatusMessage("");
        if (callback)
          callback(buf, c);
        return buf;
      }
    default:
      break;
    }
    if (!iscntrl(c) && c < 128)
    {
      if (buflen == bufsize - 1)
      {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }
    if (callback)
      callback(buf, c);
  }
}

void editorMoveCursor(int key)
{
  erow *row = (EConfig.cy >= EConfig.numrows) ? NULL : &EConfig.row[EConfig.cy];
  switch (key)
  {
  case MoveLeft:
    if (EConfig.cx > 0)
    {
      EConfig.cx--;
    }
    else if (EConfig.cy > 0)
    {
      EConfig.cy--;
      EConfig.cx = EConfig.row[EConfig.cy].size;
    }
    break;
  case MoveRight:
    // if (EConfig.cx < EConfig.screencols - 1)
    // {
    if (row && EConfig.cx < row->size)
    {
      EConfig.cx++;
    }
    else
    {
      EConfig.cy++;
      EConfig.cx = 0;
    }
    // }
    break;
  case MoveUp:
    if (EConfig.cy > 0)
    {
      EConfig.cy--;
    }
    break;
  case MoveDown:
    // if (EConfig.cy < EConfig.screenrows - 1)
    if (EConfig.cy < EConfig.numrows)
    {
      EConfig.cy++;
    }
    break;
  }

  row = (EConfig.cy >= EConfig.numrows) ? NULL : &EConfig.row[EConfig.cy];
  int rowlen = row ? row->size : 0;
  if (EConfig.cx > rowlen)
    EConfig.cx = rowlen;
}

void editorProcessKeypress()
{
  int c = editorReadKey();
  int times = 0;
  switch (c)
  {
  case '\r':
    editorInsertNewline();
    break;

  case CTRL_KEY('b'):
    if (EConfig.dirty && !EConfig.prompt)
    {
      EConfig.prompt = 1;
      editorSetStatusMessage("Are you sure you want to close without saving? Y or N");
      return;
    }
    editorClose();
    break;
  case CTRL_KEY('s'):
    editorSave();
    break;
  case CTRL_KEY('f'):
    editorFind();
    break;
  case MoveEndOfLine:
    if (EConfig.cy < EConfig.numrows)
      EConfig.cx = EConfig.row[EConfig.cy].size;
    break;
  case MoveStartOfLine:
    EConfig.cx = 0;
    break;
  case Backspace:
  case CTRL_KEY('h'):
  case Delete:
    if (c == Delete)
      editorMoveCursor(MoveRight);
    // else if (c == Backspace)
    //   editorMoveCursor(MoveLeft);
    editorDelChar();
    break;
  case PageUp:
    EConfig.cy = EConfig.rowoff;
    times = EConfig.screenrows;
    while (times--)
      editorMoveCursor(MoveUp);
    break;
  case PageDown:
    EConfig.cy = EConfig.rowoff + EConfig.screenrows - 1;
    if (EConfig.cy > EConfig.numrows)
      EConfig.cy = EConfig.numrows;

    times = EConfig.screenrows;
    while (times--)
      editorMoveCursor(MoveUp);
    break;
  case MoveLeft:
  case MoveUp:
  case MoveRight:
  case MoveDown:
    editorMoveCursor(c);
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;
  case 'n':
    if (EConfig.prompt)
    {
      EConfig.prompt = 0;
      editorClose();
      return;
    }
  case 'y':
    if (EConfig.prompt)
    {
      editorSave();
      EConfig.prompt = 0;
      editorClose();
      return;
    }
  default:
    editorInsertChar(c);
    break;
  }
}

/*** Init ***/
void initEditor()
{
  EConfig.cx = 0;
  EConfig.cy = 0;
  EConfig.rx = 0;
  EConfig.numrows = 0;
  EConfig.rowoff = 0;
  EConfig.coloff = 0;
  EConfig.row = NULL;
  EConfig.filename = NULL;
  EConfig.statusmsg[0] = '\0';
  EConfig.statusmsg_time = 0;
  EConfig.dirty = 0;
  EConfig.syntax = NULL;
  if (getWindowSize(&EConfig.screenrows, &EConfig.screencols) == -1)
    die("getWindowSize");
  EConfig.screenrows -= 2;
}

int main(int argc, char *argv[])
{
  enableRawMode();
  initEditor();
  if (argc >= 2)
  {
    // printf(argv[1]);
    editorOpen(argv[1]);
  }
  editorSetStatusMessage("HELP: Ctrl-F = Find | Ctrl-S = save | Ctrl-B = quit");
  while (1)
  {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
