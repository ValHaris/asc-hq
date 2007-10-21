#include "paragui.h"
#include "pglayout.h"
 
#include "pgwidget.h"
#include "pgbutton.h"
#include "pgdropdown.h"
#include "pglabel.h"
#include "pgthemewidget.h"
#include "pgmaskedit.h"
#include "pgprogressbar.h"
#include "pgspinnerbox.h"
#include "pgwindow.h"
#include "pgwidgetlist.h"
#include "pglistbox.h"
#include "pglistboxitem.h"
#include "pgcolumnitem.h"
#include "pgmessagebox.h"
#include "pgradiobutton.h"
#include "pgcheckbutton.h"
#include "pgscrollbar.h"
#include "pgslider.h"
#include "pgimage.h"
#include "pgrichedit.h"
#include "pglog.h"
#include "pgpopupmenu.h"
#include "pgmenubar.h"
#include "pgscrollwidget.h"

#include <cstring>
#include <stdarg.h>
#include <sys/stat.h>
#include <expat.h>

// Expat UNICODE workaround - copied from xmltchar.h (Expat dist) - Inserted by Ales Teska
#ifdef XML_UNICODE
#ifndef XML_UNICODE_WCHAR_T
#error UNICODE version requires a 16-bit Unicode-compatible wchar_t=20
#endif
#define T(x) L ## x
#define tcscmp wcscmp
#else /* not XML_UNICODE */
#define T(x) x
#define tcscmp strcmp
#endif

#define IsTag(lx,sx,y)	(((tcscmp(T(lx),T(name))==0)||(tcscmp(T(sx),T(name))==0))&&((XMLParser->Section & (y))!=0))

#define XML_BUFF_SIZE				1024

#define XML_SECTION_NONPAIR			0x00000000
#define XML_SECTION_DOC				0x00000001
#define XML_SECTION_LAYOUT			0x00000002
#define XML_SECTION_HEAD			0x00000004
#define XML_SECTION_BODY			0x00000008
#define XML_SECTION_BUTTON			0x00000010
#define XML_SECTION_COMWIDPARAMS	0x00000020
#define XML_SECTION_DROPDOWN		0x00000040
#define XML_SECTION_WIDGET			0x00000080
#define XML_SECTION_LABEL			0x00000100
#define XML_SECTION_GWIDGET			0x00000200
#define XML_SECTION_SFRAME			0x00000400
#define XML_SECTION_LINEEDIT		0x00000800
#define XML_SECTION_MASKEDIT		0x00001000
#define XML_SECTION_PROGRESSBAR		0x00002000
#define XML_SECTION_SPINNERBOX		0x00004000
#define XML_SECTION_WINDOW			0x00008000
#define XML_SECTION_WIDGETLIST		0x00010000
#define XML_SECTION_LISTBOX			0x00020000
#define XML_SECTION_LISTBOXITEM		0x00040000
#define XML_SECTION_COLUMNITEM		0x00080000
#define XML_SECTION_MESSAGEBOX		0x00100000
#define XML_SECTION_RADIOBUTTON		0x00200000
#define XML_SECTION_CHECKBUTTON		0x00400000
#define XML_SECTION_SCROLLBAR		0x00800000
#define XML_SECTION_IMAGE			0x01000000
#define XML_SECTION_RICHEDIT		0x02000000
#define XML_SECTION_WIDGETLISTEX	0x04000000
#define XML_SECTION_FONT			0x08000000
#define XML_SECTION_POPUPMENU		0x10000000
#define XML_SECTION_MENUBAR			0x20000000
#define XML_SECTION_POPUPMENUITEM	0x40000000

typedef struct {
	XML_Parser	Parser;
	int			Section;

	const char* FileName;
	int			EndTagFlags;			//Not inherited
	int			InhTagFlags;			//Inherited
	int			Height;
	int			Width;

	PG_Widget	*ParentObject;
	void		*PrevUserData;			//Previous user data
	void		*UserSpace;

}
ParseUserData_t;


#define ENDTAGFLAG_OBJECT			1
#define ENDTAGFLAG_WIDGETLIST		2
#define ENDTAGFLAG_SETSIZE			4

#define INHTAGFLAG_HIDE				1
#define INHTAGFLAG_ADDWIDGET		2

static char Empty = 0;

char *PG_Layout::GetParamStr(const char **Source, const char *What) {
	char		**c;

	for (c = (char **)Source;*c; c += 2)
		if (tcscmp(T(*c),T(What)) == 0)
			return(*(c+1));
	return(&Empty);
}

void PG_Layout::GetParamRect(const char **Source, const char *What, PG_Rect& Rect, PG_Widget* parent) {
	char *c = PG_Layout::GetParamStr(Source,What);

	if(c == NULL) {
		return;
	}

	if(strlen(c) == 0) {
		return;
	}

	SDL_Surface *screen = PG_Application::GetScreen();
	char* parm;
	char *d;
	char tmp[16];
	int i=0;
	int mx;
	int r[4];

	r[0] = r[1] = r[2] = r[3] = 0;
	parm = strdup(c);

	for(d = strtok(parm,","); d != NULL; d = strtok(NULL,",")) {
		if(parent == NULL) {
			mx = ((i%2)==0) ? screen->w : screen->h;
		} else {
			mx = ((i%2)==0) ? parent->w : parent->h;
		}

		if(  sscanf(d, "%d%[%]", & r[i], tmp) == 2 )
			r[i] = (int)  ((float)r[i]*mx/100);

		if(r[i]<0)
			r[i] = mx+r[i];

		i++;
	}
	Rect.SetRect(r[0], r[1], r[2], r[3]);
	free(parm);
}

int PG_Layout::GetParamInt(const char **Source, const char *What) {
	char* p = PG_Layout::GetParamStr(Source, What);

	if(p[0] == 0) {
		return -1;
	}

	return atoi(p);
}

PG_ScrollBar::ScrollDirection PG_Layout::GetParamScrollDirection(const char **Source, const char *What) {
	char* p = PG_Layout::GetParamStr(Source, What);
	PG_ScrollBar::ScrollDirection r = PG_ScrollBar::VERTICAL;

	switch(atoi(p)) {
		case 0:
			r = PG_ScrollBar::VERTICAL;
			break;

		case 1:
			r = PG_ScrollBar::HORIZONTAL;
			break;
	}

	return r;
}
PG_Label::TextAlign PG_Layout::GetParamAlign(const char **Source, const char *What) {
	PG_Label::TextAlign ret = PG_Label::LEFT;
	char *c = PG_Layout::GetParamStr(Source,What);

	if (c[0]==0)
		return PG_Label::LEFT;

	if (tcscmp(T(c),T("left")) == 0)
		ret = PG_Label::LEFT;
	if (tcscmp(T(c),T("right")) == 0)
		ret = PG_Label::RIGHT;
	if (tcscmp(T(c),T("center")) == 0)
		ret = PG_Label::CENTER;

	return ret;
}

PG_Draw::BkMode PG_Layout::GetParamIMode(const char **Source, const char *What) {
	PG_Draw::BkMode	ret = PG_Draw::TILE;
	char *c = PG_Layout::GetParamStr(Source,What);

	if (tcscmp(T(c),T("tile")) == 0)
		ret = PG_Draw::TILE;
	if (tcscmp(T(c),T("stretch")) == 0)
		ret = PG_Draw::STRETCH;
	if (tcscmp(T(c),T("3tileh")) == 0)
		ret = PG_Draw::TILE3H;
	if (tcscmp(T(c),T("3tilev")) == 0)
		ret = PG_Draw::TILE3V;
	if (tcscmp(T(c),T("9tile")) == 0)
		ret = PG_Draw::TILE9;

	return(ret);
}

int PG_Layout::GetParamGrad(const char **Source, const char *What, PG_Gradient *grad) {
	char	*c = PG_Layout::GetParamStr(Source,What);
	int		r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4;

	if (c[0] == 0)
		return(0);

	sscanf(c,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&r1,&g1,&b1,&r2,&g2,&b2,&r3,&g3,&b3,&r4,&g4,&b4);

	grad->colors[0].r = r1;
	grad->colors[0].g = g1;
	grad->colors[0].b = b1;

	grad->colors[1].r = r2;
	grad->colors[1].g = g2;
	grad->colors[1].b = b2;

	grad->colors[2].r = r3;
	grad->colors[2].g = g3;
	grad->colors[2].b = b3;

	grad->colors[3].r = r4;
	grad->colors[3].g = g4;
	grad->colors[3].b = b4;

	return(1);
}

static void SaveUserData(ParseUserData_t *XMLParser) {
	ParseUserData_t	*OldUserData;

	OldUserData = (ParseUserData_t	*)malloc(sizeof(ParseUserData_t));

	*OldUserData = *XMLParser;

	XMLParser->PrevUserData = OldUserData;
	XMLParser->EndTagFlags = 0;
	return;
}

static void RestoreUserData(ParseUserData_t *XMLParser) {
	ParseUserData_t	*OldUserData = (ParseUserData_t	*)XMLParser->PrevUserData;

	*XMLParser = *OldUserData;
	free(OldUserData);

	return;
}


void (* PG_LayoutWidgetParams)(PG_Widget *Widget, const char **atts) = NULL;
void (* PG_LayoutProcessingInstruction)(const char* target,  const char* data, const std::string& FileName, void *UserSpace) = NULL;

//Declaration
//static void XMLTextDoc(void *userData, const XML_Char *s, int len);

static int SetWidgetAtts(PG_Widget *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char	*c;
	int		i,ret = 0;

	c = PG_Layout::GetParamStr(atts, "name");
	if (c[0] != 0)
		Widget->SetName(c);

	i = PG_Layout::GetParamInt(atts, "id");
	if (i != -1) {
		Widget->SetID(i);
	}

	c = PG_Layout::GetParamStr(atts, "fcolor");
	if (c[0] != 0) {
		int	r,g,b;

		sscanf(c,"%d,%d,%d",&r,&g,&b);
		Widget->SetFontColor(PG_Color(r,g,b));
	}

	c = PG_Layout::GetParamStr(atts, "fname");
	if (c[0] != 0) {
		Widget->SetFontName(c);
	}

	i = PG_Layout::GetParamInt(atts, "fsize");
	if (i != -1) {
		Widget->SetFontSize(i);
	}

	i = PG_Layout::GetParamInt(atts, "fstyle");
	if (i != -1) {
		Widget->SetFontStyle((PG_Font::Style)i);
	}

	i = PG_Layout::GetParamInt(atts, "findex");
	if (i != -1) {
		Widget->SetFontIndex(i);
	}

	i = PG_Layout::GetParamInt(atts, "falpha");
	if (i != -1) {
		Widget->SetFontAlpha(i);
	}

	c = PG_Layout::GetParamStr(atts, "data");
	if (c[0] != 0)
		Widget->SetUserData(c, strlen(c)+1);

	if (PG_LayoutWidgetParams != NULL)
		PG_LayoutWidgetParams(Widget,atts);

	//TO-DO : Remove this (obsolete)
	c = PG_Layout::GetParamStr(atts, "text");
	if (c[0] != 0) {
		Widget->SetText((const char*)c);
	}

	i = PG_Layout::GetParamInt(atts, "hide");
	if (i == 1)
		ret |= INHTAGFLAG_HIDE;

	c = PG_Layout::GetParamStr(atts, "sbt");
	if (c[0] != 0) {
		sscanf(c,"%d,%d", &XMLParser->Width, &XMLParser->Height);
		XMLParser->EndTagFlags |= ENDTAGFLAG_SETSIZE;
	}

	//Remove AddWidget tag ...
	ret &= ~INHTAGFLAG_ADDWIDGET;

	XMLParser->EndTagFlags |= ENDTAGFLAG_OBJECT;
	return(ret);
}

static int SetDropDownAtts(PG_DropDown *Widget, const char **atts,ParseUserData_t *XMLParser) {
	int		i;

	i = PG_Layout::GetParamInt(atts, "indent");
	if (i > 0)
		Widget->SetIndent(i);

	i = PG_Layout::GetParamInt(atts, "edit");
	Widget->SetEditable(i == 1);

	return (SetWidgetAtts(Widget, atts, XMLParser));
}

static int SetThemeWidgetAtts(PG_ThemeWidget *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char		*c;
	PG_Gradient	grad;

	c = PG_Layout::GetParamStr(atts, "image");
	if(c[0] != 0) {
		Widget->SetBackground(c,PG_Layout::GetParamIMode(atts, "imode"));
	}

	int b = PG_Layout::GetParamInt(atts, "blend");
	if(b != -1) {
		Widget->SetBackgroundBlend(b);
	}

	if (PG_Layout::GetParamGrad(atts, "gradient", &grad) != 0)
		Widget->SetGradient(grad);

	c = PG_Layout::GetParamStr(atts, "bimage");
	if (c[0] != 0) {
		Widget->LoadImage(c);
	}

	b = PG_Layout::GetParamInt(atts, "transparency");

	if(b != -1) {
		Widget->SetTransparency(b);
	}

	return SetWidgetAtts(Widget, atts, XMLParser);
}

static int SetLabelAtts(PG_Label *Widget, const char **atts, ParseUserData_t *XMLParser) {
	PG_Label::TextAlign a;
	int i;
	char* c;

	a = PG_Layout::GetParamAlign(atts, "align");
	Widget->SetAlignment(a);

	i = PG_Layout::GetParamInt(atts, "indent");
	if (i != -1) {
		Widget->SetIndent(i);
	}

	c = PG_Layout::GetParamStr(atts, "icon");
	if (c[0] != 0) {
		Widget->SetIcon((const char*)c);
	}

	return(SetWidgetAtts(Widget, atts, XMLParser));
}

static int SetUserButtonAtts(PG_Button *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char	*c,*c1,*c2;
	int i,j,k;
	int b;

	c = PG_Layout::GetParamStr(atts, "upimage");
	c1 = PG_Layout::GetParamStr(atts, "downimage");
	c2 = PG_Layout::GetParamStr(atts, "overimage");
	/*if (c1[0] == 0) {
		c1 = NULL;
	}

	if (c2[0] == 0) {
		c2 = NULL;
	}*/

	if (c[0] != 0) {
		b = PG_Layout::GetParamInt(atts, "colorkey");
		if(b != -1) {
			Widget->SetIcon(c, c1, c2, b);
		} else {
			Widget->SetIcon(c, c1, c2);
		}
	}

	i = PG_Layout::GetParamInt(atts, "toggle");
	Widget->SetToggle(i == 1);

	i = PG_Layout::GetParamInt(atts, "pressed");
	Widget->SetPressed(i == 1);

	c = PG_Layout::GetParamStr(atts, "shift");
	if (c[0] != 0) {
		sscanf(c,"%d",&i);
		Widget->SetShift(i);
	}

	c = PG_Layout::GetParamStr(atts, "border");
	if (c[0] != 0) {
		sscanf(c,"%d,%d,%d",&i,&j,&k);
		Widget->SetBorderSize(i,j,k);
	}

	c = PG_Layout::GetParamStr(atts, "transparency");
	if (c[0] != 0) {
		sscanf(c,"%d,%d,%d",&i,&j,&k);
		Widget->SetTransparency(i,j,k);
	}

	return(SetWidgetAtts(Widget, atts, XMLParser));
}

static int SetButtonAtts(PG_Button *Widget, const char **atts, ParseUserData_t *XMLParser) {
	PG_Gradient grad;

	if (PG_Layout::GetParamGrad(atts, "upgrad", &grad) != 0)
		Widget->SetGradient(PG_Button::UNPRESSED, grad);
	if (PG_Layout::GetParamGrad(atts, "downgrad", &grad) != 0)
		Widget->SetGradient(PG_Button::PRESSED, grad);
	if (PG_Layout::GetParamGrad(atts, "selgrad", &grad) != 0)
		Widget->SetGradient(PG_Button::HIGHLITED, grad);

	/*SetBackground*/

	return(SetUserButtonAtts(Widget, atts, XMLParser));
}

static int SetPopupMenuAtts(PG_PopupMenu *Widget, const char **atts, ParseUserData_t *XMLParser) {

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static int SetLineEditAtts(PG_LineEdit *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char	*c;

	if (PG_Layout::GetParamInt(atts, "readonly") == 1)
		Widget->SetEditable(0);

	c = PG_Layout::GetParamStr(atts, "validkeys");
	if (c[0] != 0)
		Widget->SetValidKeys(c);

	c = PG_Layout::GetParamStr(atts, "passchar");
	if (c[0] != 0)
		Widget->SetPassHidden(c[0]);

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static int SetMaskEditAtts(PG_MaskEdit *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char	*c;
	Widget->SetMask(PG_Layout::GetParamStr(atts, "mask"));

	c = PG_Layout::GetParamStr(atts, "spacer");
	if (c[0] != 0)
		Widget->SetSpacer(c[0]);

	return(SetLineEditAtts(Widget, atts, XMLParser));
}

static int SetWindowAtts(PG_Window *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char *c;

	c = PG_Layout::GetParamStr(atts, "titlecolor");
	if (c[0] != 0) {
		int r,g,b;
		sscanf(c,"%d,%d,%d",&r,&g,&b);
		Widget->SetTitlebarColor(((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xFF));
	}

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static int SetProgressBarAtts(PG_ProgressBar *Widget, const char **atts, ParseUserData_t *XMLParser) {
	char	*c;
	double	p;

	c = PG_Layout::GetParamStr(atts, "progress");
	if (c[0] != 0) {
		p = atof(c);
		Widget->SetProgress(p);
	}

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static int SetSpinnerBoxAtts(PG_SpinnerBox *Widget, const char **atts, ParseUserData_t *XMLParser) {
	int		i,j;
	char	*c;

	i = PG_Layout::GetParamInt(atts, "value");
	if(i != -1) {
		Widget->SetValue(i);
	}

	i = PG_Layout::GetParamInt(atts, "min");
	if(i != -1) {
		Widget->SetMinValue(i);
	}

	j = PG_Layout::GetParamInt(atts, "max");
	if (j > i)
		Widget->SetMaxValue(j);

	c = PG_Layout::GetParamStr(atts, "mask");
	if (c[0] != 0)
		Widget->SetMask(c);

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static int SetWidgetListAtts(PG_WidgetList *Widget, const char **atts, ParseUserData_t *XMLParser) {
	if (PG_Layout::GetParamInt(atts, "scrollbar") == 1)
		Widget->EnableScrollBar(1);

	XMLParser->EndTagFlags |= ENDTAGFLAG_WIDGETLIST;
	return (INHTAGFLAG_ADDWIDGET | SetThemeWidgetAtts(Widget, atts, XMLParser));
}

static int SetScrollWidgetAtts(PG_ScrollWidget*Widget, const char **atts, ParseUserData_t *XMLParser) {
	if (PG_Layout::GetParamInt(atts, "scrollbar") == 1)
		Widget->EnableScrollBar(1);

	XMLParser->EndTagFlags |= ENDTAGFLAG_WIDGETLIST;
	return (INHTAGFLAG_ADDWIDGET | SetThemeWidgetAtts(Widget, atts, XMLParser));
}

static int SetListBoxAtts(PG_ListBox *Widget, const char **atts, ParseUserData_t *XMLParser) {
	Widget->SetMultiSelect(PG_Layout::GetParamInt(atts, "multisel") == 1);

	return(SetWidgetListAtts(Widget, atts, XMLParser));
}

static int SetListBoxItemAtts(PG_ListBoxItem *Widget, const char **atts, ParseUserData_t *XMLParser) {
	if (PG_Layout::GetParamInt(atts, "select") == 1)
		Widget->Select(1);

	return(SetLabelAtts(Widget, atts, XMLParser));
}

static int SetRadioButtonAtts(PG_RadioButton *Widget, const char **atts, ParseUserData_t *XMLParser) {
	if (PG_Layout::GetParamInt(atts, "pressed") == 1)
		Widget->SetPressed();

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}


static int SetScrollBarAtts(PG_ScrollBar *Widget, const char **atts, ParseUserData_t *XMLParser) {
	int	i,j;

	i = PG_Layout::GetParamInt(atts, "value");
	if (i != -1) {
		Widget->SetPosition(i);
	}

	/*i = PG_Layout::GetParamInt(atts, "wsize");
	if (i != -1) {
		Widget->SetWindowSize(i);
	}*/

	i = PG_Layout::GetParamInt(atts, "lsize");
	if (i != -1) {
		Widget->SetLineSize(i);
	}

	i = PG_Layout::GetParamInt(atts, "psize");
	if (i != -1) {
		Widget->SetPageSize(i);
	}

	i = PG_Layout::GetParamInt(atts, "min");
	j = PG_Layout::GetParamInt(atts, "max");
	if ((i < j) && (i != j)) {
		Widget->SetRange(i,j);
	}

	return SetThemeWidgetAtts(Widget, atts, XMLParser);
}

static void XMLStartDoc(void *userData, const char *name, const char **atts) {
	PG_Rect Rect;		// -> UserData ???? - for parent size
	ParseUserData_t	*XMLParser = (ParseUserData_t	*)userData;

	SaveUserData(XMLParser);
	PG_Widget* parent = /*((XMLParser->EndTagFlags & INHTAGFLAG_ADDWIDGET) == INHTAGFLAG_ADDWIDGET) ? NULL :*/ XMLParser->ParentObject;

	//Tag <layout> <LA>
	if (IsTag("layout","LA",XML_SECTION_DOC)) {
		XMLParser->Section = XML_SECTION_LAYOUT;
		return;
	}

	//Tag <head> <HD>
	if (IsTag("head","HD",XML_SECTION_LAYOUT)) {
		XMLParser->Section = XML_SECTION_HEAD;
		return;
	}

	//Tag <body> <BD>
	if (IsTag("body","BD",XML_SECTION_LAYOUT)) {
		XMLParser->Section = XML_SECTION_BODY;
		return;
	}

	//Tag <widget> <WD>
	if (IsTag("widget","WD",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_WIDGET | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_Widget	*Widget = new PG_Widget(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetWidgetAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <dropdown> <DD>
	if (IsTag("dropdown","DD",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_DROPDOWN | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		if (Rect.w < Rect.h) {
			PG_LogWRN("Width must be greater than height in <dropdown>");
			return;
		}

		PG_DropDown *Widget = new PG_DropDown (parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetDropDownAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <dropdownitem> <DI>
	if (IsTag("dropdownitem","DI",XML_SECTION_DROPDOWN)) {
		XMLParser->Section = XML_SECTION_NONPAIR;

		((PG_DropDown *)(XMLParser->ParentObject))->AddItem(PG_Layout::GetParamStr(atts, "text"));
		return;
	}

	//Tag <button> <BT>
	if (IsTag("button","BT",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_BUTTON | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_LogDBG("layout: parent = %p", parent);
		PG_Button	*Widget = new PG_Button(
		                        parent,
		                        Rect,
		                        PG_Layout::GetParamStr(atts, "text"),
		                        PG_Layout::GetParamInt(atts, "id"),
		                        PG_Layout::GetParamStr(atts, "style"));

		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetButtonAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <label> <LL>
	if (IsTag("label","LL",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_LABEL | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_Label	*Widget = new PG_Label(parent, Rect, "", PG_Layout::GetParamStr(atts, "style"));
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetLabelAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <gradientwidget> <GW>
	//Tag <staticframe> <SF>
	//Tag <themewidget> <TW>
	if (IsTag("themewidget","GW",XML_SECTION_BODY) || IsTag("gradientwidget","GW",XML_SECTION_BODY) || IsTag("staticframe","SF",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_GWIDGET | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_ThemeWidget* Widget = new PG_ThemeWidget(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetThemeWidgetAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <lineedit> <LE>
	if (IsTag("lineedit","LE",XML_SECTION_BODY)) {
		int	i;
		XMLParser->Section = XML_SECTION_LINEEDIT | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		i = PG_Layout::GetParamInt(atts, "length");
		if (i == -1) {
			i = 1000000;
		}

		PG_LineEdit* Widget = new PG_LineEdit(parent, Rect, "LineEdit", i);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetLineEditAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <maskedit> <ME>
	if (IsTag("maskedit","ME",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_MASKEDIT | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_MaskEdit* Widget = new PG_MaskEdit(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetMaskEditAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <progressbar> <PB>
	if (IsTag("progressbar","PB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_PROGRESSBAR | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_ProgressBar *Widget = new PG_ProgressBar(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetProgressBarAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <spinnerbox> <SB>
	if (IsTag("spinnerbox","SB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_SPINNERBOX | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_SpinnerBox *Widget = new PG_SpinnerBox(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetSpinnerBoxAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <window> <WN>
	if (IsTag("window","WN",XML_SECTION_BODY)) {
		int	i;

		XMLParser->Section = XML_SECTION_WINDOW | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		i = PG_Layout::GetParamInt(atts, "titleheight");
		if (i == -1) {
			i = 25;
		}

		int flags = PG_Layout::GetParamInt(atts, "flags");
		if(flags == -1) {
			flags = 0;
		}

		PG_Window *Widget = new PG_Window(parent, Rect, PG_Layout::GetParamStr(atts, "title"), (PG_Window::WindowFlags)flags, "Window", i);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetWindowAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <messagebox> <MB>
	if (IsTag("messagebox","MB",XML_SECTION_BODY)) {
		PG_Rect B1Rect,B2Rect;
		char* B1Text,*B2Text;
		PG_MessageBox	*Widget;

		XMLParser->Section = XML_SECTION_MESSAGEBOX | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		B1Text = PG_Layout::GetParamStr(atts, "b1text");
		B2Text = PG_Layout::GetParamStr(atts, "b2text");

		PG_Layout::GetParamRect(atts, "b1pos", B1Rect, parent);
		PG_Layout::GetParamRect(atts, "b2pos", B2Rect, parent);

		if (B2Text[0] == 0) {
			Widget = new PG_MessageBox(parent, Rect, PG_Layout::GetParamStr(atts, "title"), "", B1Rect, B1Text, PG_Layout::GetParamAlign(atts, "talign"));
		} else {
			Widget = new PG_MessageBox(parent, Rect, PG_Layout::GetParamStr(atts, "title"), "", B1Rect, B1Text, B2Rect, B2Text, PG_Layout::GetParamAlign(atts, "talign"));
		}

		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetWindowAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <widgetlist> <WL>
	if (IsTag("widgetlist","WL",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_WIDGETLIST | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_WidgetList *Widget = new PG_WidgetList(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetWidgetListAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <listbox> <LB>
	if (IsTag("listbox","LB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_LISTBOX | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_ListBox *Widget = new PG_ListBox(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetListBoxAtts(Widget, atts, XMLParser) &~INHTAGFLAG_ADDWIDGET;
		return;
	}

	//Tag <listboxitem> <LI>
	if (IsTag("listboxitem","LI",XML_SECTION_LISTBOX)) {
		XMLParser->Section = XML_SECTION_LISTBOXITEM | XML_SECTION_COMWIDPARAMS;

		int h = PG_Layout::GetParamInt(atts, "height");
		if(h < 0) {
			h = 25;
		}

		// to be updated!
		PG_ListBoxItem *Widget = new PG_ListBoxItem(parent, h, PG_Layout::GetParamStr(atts, "ltext"));
		//((PG_ListBox *)XMLParser->ParentObject)->AddItem(Widget);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetListBoxItemAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <columnitem> <CI>
	if (IsTag("columnitem","CI",XML_SECTION_LISTBOX)) {
		XMLParser->Section = XML_SECTION_COLUMNITEM | XML_SECTION_COMWIDPARAMS;

		// to be updated!
		PG_ColumnItem *Widget = new PG_ColumnItem(parent, PG_Layout::GetParamInt(atts, "columns"),PG_Layout::GetParamInt(atts, "height"));
		//((PG_ListBox *)XMLParser->ParentObject)->AddItem(Widget);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetListBoxItemAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <column> <CO>
	if (IsTag("column","CO",XML_SECTION_COLUMNITEM)) {
		int	no,i;
		XMLParser->Section = XML_SECTION_COLUMNITEM | XML_SECTION_COMWIDPARAMS;

		no = PG_Layout::GetParamInt(atts, "number");
		if(no == -1) {
			no = 0;
		}

		if (no >= ((PG_ColumnItem *)XMLParser->ParentObject)->GetColumnCount ()) {
			PG_LogWRN("Column number error !!!");
			return;
		}

		i = PG_Layout::GetParamInt(atts, "width");
		if (i != -1) {
			((PG_ColumnItem *)XMLParser->ParentObject)->SetColumnWidth(no, i);
		}

		((PG_ColumnItem *)XMLParser->ParentObject)->SetColumnText(no, PG_Layout::GetParamStr(atts, "text"));

		return;
	}

	//Tag <radiobutton> <RB>
	if (IsTag("radiobutton","RB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_RADIOBUTTON | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_RadioButton *Widget = new PG_RadioButton(
		                             parent,
		                             Rect,
		                             (const char*)PG_Layout::GetParamStr(atts, "text"),
		                             (PG_RadioButton *)PG_Application::GetWidgetByName(PG_Layout::GetParamStr(atts, "group")),
		                             0);

		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetRadioButtonAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <checkbutton> <CB>
	if (IsTag("checkbutton","CB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_CHECKBUTTON | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_CheckButton *Widget = new PG_CheckButton(
		                             parent,
		                             Rect,
		                             PG_Layout::GetParamStr(atts, "text"),
		                             0);

		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetRadioButtonAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <scrollbar> <SB>
	if (IsTag("scrollbar","SB",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_SCROLLBAR | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_ScrollBar::ScrollDirection d = PG_Layout::GetParamScrollDirection(atts, "dir");
		(d <= 0) ? d = PG_ScrollBar::HORIZONTAL : d = PG_ScrollBar::VERTICAL;

		PG_ScrollBar *Widget = new PG_ScrollBar(parent, Rect, d);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetScrollBarAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <slider> <SR>
	if (IsTag("slider","SR",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_SCROLLBAR | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_Slider *Widget = new PG_Slider(parent, Rect, PG_Layout::GetParamScrollDirection(atts, "dir"));
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |=SetScrollBarAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <image> <IM>
	if (IsTag("image","IM",XML_SECTION_BODY)) {
		PG_Point Point;
		PG_Rect Rect;

		XMLParser->Section = XML_SECTION_IMAGE | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		Point.x=Rect.x;
		Point.y=Rect.y;

		PG_Image *Widget = new PG_Image(parent, Point, PG_Layout::GetParamStr(atts, "iimage"));
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetThemeWidgetAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <richedit> <RE>
	if (IsTag("richedit","RE",XML_SECTION_BODY)) {
		XMLParser->Section = XML_SECTION_RICHEDIT | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_RichEdit *Widget = new PG_RichEdit(parent, Rect, PG_Layout::GetParamInt(atts, "vresize") == 1 , PG_Layout::GetParamInt(atts, "linewidth"));
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetScrollWidgetAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <popupmenu> <PM>
	if(IsTag("popupmenu", "PM", XML_SECTION_BODY)) {
		PG_Point Point;
		PG_Rect Rect;

		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		Point.x=Rect.x;
		Point.y=Rect.y;

		PG_PopupMenu* Widget = NULL;

		// is the current popup a child of another popup ?
		if(XMLParser->Section & XML_SECTION_POPUPMENU) {
			Widget = new PG_PopupMenu(NULL, 0, 0, PG_NULLSTR);
			PG_PopupMenu* menu = static_cast<PG_PopupMenu*>(XMLParser->ParentObject);
			menu->addMenuItem(PG_Layout::GetParamStr(atts, "caption"), Widget);
		} else {
			// is the popup a child of a menubar ?
			if(XMLParser->Section & XML_SECTION_MENUBAR) {
				Widget = new PG_PopupMenu(NULL, Point.x, Point.y, PG_Layout::GetParamStr(atts, "caption"));
				static_cast<PG_MenuBar*>(XMLParser->ParentObject)->Add(PG_Layout::GetParamStr(atts, "text"), Widget);
			} else {
				Widget = new PG_PopupMenu(parent, Point.x, Point.y, PG_Layout::GetParamStr(atts, "caption"));
			}
		}

		// register section popupmenu
		XMLParser->Section = XML_SECTION_POPUPMENU | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;

		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetPopupMenuAtts(Widget, atts, XMLParser);
		XMLParser->InhTagFlags |= INHTAGFLAG_HIDE; // Popups shouldn't be displayed immediately
		return;
	}

	//Tag <popupmenuitem> <PM>
	if(IsTag("popupmenuitem", "PI", XML_SECTION_BODY)) {
		if(!(XMLParser->Section & XML_SECTION_POPUPMENU)) {
			PG_LogERR("popupmenuitem can't be created in the current context!");
			return;
		}
		PG_PopupMenu* menu = static_cast<PG_PopupMenu*>(XMLParser->ParentObject);
		menu->addMenuItem(PG_Layout::GetParamStr(atts, "caption"), PG_Layout::GetParamInt(atts, "id"));

		return;
	}

	//Tag <menubar> <MB>
	if(IsTag("menubar", "MB", XML_SECTION_BODY)) {
		PG_Rect Rect;

		XMLParser->Section = XML_SECTION_MENUBAR | XML_SECTION_BODY | XML_SECTION_COMWIDPARAMS;
		PG_Layout::GetParamRect(atts, "pos", Rect, parent);

		PG_MenuBar* Widget = new PG_MenuBar(parent, Rect);
		XMLParser->ParentObject = Widget;

		XMLParser->InhTagFlags |= SetThemeWidgetAtts(Widget, atts, XMLParser);
		return;
	}

	//Tag <style> <ST>
	if (IsTag("style","ST",XML_SECTION_COMWIDPARAMS)) {
		char		*widget;
		char		*object;

		XMLParser->Section = XML_SECTION_NONPAIR;

		widget = PG_Layout::GetParamStr(atts, "widget");
		if (widget[0] == 0)
			return;

		object = PG_Layout::GetParamStr(atts, "object");

		if (object[0] == 0)	{
			XMLParser->ParentObject->LoadThemeStyle(widget);
		} else {
			XMLParser->ParentObject->LoadThemeStyle(widget,object);
		}
		return;
	}

	//Tag <font> <FT>
	if (IsTag("font","FT",XML_SECTION_COMWIDPARAMS)) {
		return;
	}

	if(XMLParser->Section & XML_SECTION_HEAD) {
		// create tag description
		PG_XMLTag* n = new PG_XMLTag(name, atts);
		PG_Application::GetApp()->sigXMLTag(n);
	}

	PG_LogWRN("Unknown tag `%s` in section %d !",name,XMLParser->Section);
	return;
}

static void XMLEndDoc(void *userData, const char *name) {
	ParseUserData_t	*XMLParser = (ParseUserData_t *)userData;
	PG_Widget		*WidgetToAdd = NULL;

	//if (XMLParser->Section & XML_SECTION_FONT)
	//	XMLTextDoc(userData, PG_FFT_END_TAG , 2);

	if ((XMLParser->EndTagFlags & ENDTAGFLAG_SETSIZE) != 0) {
		XMLParser->ParentObject->SetSizeByText(XMLParser->Width,XMLParser->Height);
	}

	if ((XMLParser->EndTagFlags & ENDTAGFLAG_OBJECT) != 0) {
		if (((XMLParser->InhTagFlags & INHTAGFLAG_ADDWIDGET) != 0) && ((XMLParser->EndTagFlags & ENDTAGFLAG_WIDGETLIST) == 0)) {
			//WidgetToAdd = XMLParser->ParentObject;
			//			goto object_end;
		}

		if ((XMLParser->InhTagFlags & INHTAGFLAG_HIDE) == 0) {
			if(XMLParser->ParentObject->GetParent() == NULL) {
				XMLParser->ParentObject->Show();
			}
		} else {
			XMLParser->ParentObject->Hide();
		}

		//object_end:
		//		XMLParser->ParentObject->AddText("", true);
	}

	RestoreUserData(XMLParser);

	if (WidgetToAdd == NULL) {
		return;
	}

	((PG_WidgetList *)(XMLParser->ParentObject))->AddChild(WidgetToAdd);
}

static void XMLTextDoc(void *userData, const XML_Char *s, int len) {
	char* buffer = new char[len+1];

	ParseUserData_t	*XMLParser = (ParseUserData_t *)userData;

	memcpy(buffer,s,len);
	buffer[len]=0;

	if (XMLParser->ParentObject != NULL)
		XMLParser->ParentObject->AddText(buffer);

	delete[] buffer;
}

static void XMLProcInstr(void *userData, const XML_Char *target, const XML_Char *data) {
	ParseUserData_t	*XMLParser = (ParseUserData_t *)userData;

	if (PG_LayoutProcessingInstruction != NULL)
		PG_LayoutProcessingInstruction(target, data, XMLParser->FileName, XMLParser->UserSpace);
}

bool PG_Layout::Load(PG_Widget* parent, const std::string& filename, void (* WorkCallback)(int now, int max), void *UserSpace) {
	ParseUserData_t	XMLParser;
	bool status = true;
	int bytes_pos = 0;

	XMLParser.Parser = XML_ParserCreate(NULL);
	XMLParser.Section = XML_SECTION_DOC;
	XMLParser.PrevUserData = NULL;
	XMLParser.ParentObject = parent;
	XMLParser.InhTagFlags = 0;
	XMLParser.UserSpace = UserSpace;

	XML_SetUserData(XMLParser.Parser, &XMLParser);
	XML_SetElementHandler(XMLParser.Parser, XMLStartDoc, XMLEndDoc);
	XML_SetCharacterDataHandler(XMLParser.Parser, XMLTextDoc);
	XML_SetProcessingInstructionHandler(XMLParser.Parser, XMLProcInstr);

	XMLParser.FileName = filename.c_str();
	PG_File* f = PG_Application::OpenFile(XMLParser.FileName);
	if (f == NULL) {

		PG_LogWRN("Layout file %s not found !", filename.c_str());
		return false;
	}

	for (;;) {
		int bytes_read;
		void* buff;

		if ((buff = XML_GetBuffer(XMLParser.Parser, XML_BUFF_SIZE)) == NULL) {
			PG_LogWRN("Can`t get parse buffer");
			status = false;
			break;
		}

		bytes_read = f->read(buff, XML_BUFF_SIZE);
		bytes_pos += bytes_read;

		if (WorkCallback != NULL) {
			WorkCallback(bytes_pos , f->fileLength());
		}

		if (XML_ParseBuffer(XMLParser.Parser, bytes_read, bytes_read == 0) == 0) {
			PG_LogWRN("%s on the line %d pos %d",XML_ErrorString(XML_GetErrorCode(XMLParser.Parser)),XML_GetCurrentLineNumber(XMLParser.Parser), XML_GetCurrentColumnNumber(XMLParser.Parser));
			status = false;
			break;
		}

		if (bytes_read == 0) {
			status = true;
			break;
		}
	}

	if (XMLParser.Parser != NULL) {
		XML_ParserFree(XMLParser.Parser);
	}

	delete f;

	while (XMLParser.PrevUserData != NULL)
		RestoreUserData(&XMLParser);

	return true;
}

PG_XMLTag::PG_XMLTag(const char* n, const char** a) {
	// fill in name
	name = strdup(name);

	if(a == NULL) {
		atts = NULL;
		return;
	}

	// get size of arry
	const char** a0 = a;
	int s=0;
	while((*a0) != NULL) {
		a0++;
		s++;
	}

	// alloc array of pointers
	atts = (const char**)malloc((s+1)*sizeof(char*));
	a0 = atts;

	// copy strings to new array
	while(*a != NULL) {
		*a0 = strdup(*a);
		a0++;
		a++;
	}
	*a0 = NULL;
}

PG_XMLTag::~PG_XMLTag() {
	if(name != NULL) {
		free((void*)name);
	}

	if(atts == NULL) {
		return;
	}

	const char** a = atts;
	while(*a) {
		free((void*)*a);
		a++;
	}

	free((void*)atts);
}
