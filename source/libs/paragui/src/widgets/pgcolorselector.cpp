#include "pgcolorselector.h"

PG_ColorSelector::PG_ColorBox::PG_ColorBox(PG_ColorSelector* parent, const PG_Rect& r) : PG_ThemeWidget(parent, r) {

	SetBackground((SDL_Surface*)NULL);
	SetSimpleBackground(false);
	
	my_btndown = false;

	p.x = r.w/2;
	p.y = r.h/2;
}
		
bool PG_ColorSelector::PG_ColorBox::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	if(!my_btndown) {
		return false;
	}
	
	p = ScreenToClient(motion->x, motion->y);

	if(p.x < 0) {
		p.x = 0;
	}

	if(p.x >= my_width-1) {
		p.x = my_width-1;
	}

	if(p.y < 0) {
		p.y = 0;
	}

	if(p.y >= my_height-1) {
		p.y = my_height-1;
	}

	Update();
	GetParent()->SetBaseColor(GetBaseColor());
	return true;
}

bool PG_ColorSelector::PG_ColorBox::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	if(my_btndown) {
		return false;
	}

	SetCapture();
	my_btndown = true;
	p = ScreenToClient(button->x, button->y);
	Update();
	GetParent()->SetBaseColor(GetBaseColor());
	return true;
}

bool PG_ColorSelector::PG_ColorBox::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	if(!my_btndown) {
		return false;
	}

	ReleaseCapture();
	my_btndown = false;
	return true;
}

void PG_ColorSelector::PG_ColorBox::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	PG_ThemeWidget::eventBlit(srf, src, dst);
	
	// draw crosshair
	DrawHLine(0, p.y, w, PG_Color(255,255,255));
	DrawVLine(p.x, 0, h, PG_Color(255,255,255));
}

PG_Color PG_ColorSelector::PG_ColorBox::GetBaseColor() {
	PG_Color result;
	
	PG_Color cy1, cy2;
	PG_Color r,g,b,w;
	PG_Gradient gr = GetGradient();
	
	r = gr.colors[0];
	g = gr.colors[1];
	b = gr.colors[2];
	w = gr.colors[3];
	
	// cy1 = ( (b - r) / h ) * y
	
	cy1.r = (Uint8)((float)r.r + (((float)b.r - (float)r.r) / (float)my_height) * (float)p.y);
	cy1.g = (Uint8)((float)r.g + (((float)b.g - (float)r.g) / (float)my_height) * (float)p.y);
	cy1.b = (Uint8)((float)r.b + (((float)b.b - (float)r.b) / (float)my_height) * (float)p.y);
	
	// cy2 = ( (w - g) / h ) * y
	
	cy2.r = (Uint8)((float)g.r + (((float)w.r - (float)g.r) / (float)my_height) * (float)p.y);
	cy2.g = (Uint8)((float)g.g + (((float)w.g - (float)g.g) / (float)my_height) * (float)p.y);
	cy2.b = (Uint8)((float)g.b + (((float)w.b - (float)g.b) / (float)my_height) * (float)p.y);
	
	// result = ( (cy2 - cy1) / w ) * x

	result.r = (Uint8)((float)cy1.r + (((float)cy2.r - (float)cy1.r) / (float)my_width) * (float)p.x);
	result.g = (Uint8)((float)cy1.g + (((float)cy2.g - (float)cy1.g) / (float)my_width) * (float)p.x);
	result.b = (Uint8)((float)cy1.b + (((float)cy2.b - (float)cy1.b) / (float)my_width) * (float)p.x);
	
	return result;
}

PG_ColorSelector::PG_ColorSelector(PG_Widget* parent, const PG_Rect&r, const std::string& style) : PG_ThemeWidget(parent, r, style)  {
	PG_Gradient g;

	my_color.r = 255;
	my_color.g = 255;
	my_color.b = 255;

	my_colorbox = new PG_ColorBox(this, PG_Rect(5,5,r.h-10,r.h-10));
	
	g.colors[0].r = 255;
	g.colors[0].g = 255;
	g.colors[0].b = 0;
	
	g.colors[1].r = 0;
	g.colors[1].g = 255;
	g.colors[1].b = 255;
	
	g.colors[2].r = 255;
	g.colors[2].g = 0;
	g.colors[2].b = 255;
	
	g.colors[3].r = 255;
	g.colors[3].g = 255;
	g.colors[3].b = 255;
	
	my_colorbox->SetGradient(g);

	my_colorslider = new PG_Slider(this, PG_Rect(r.h, 5, 20, r.h-10), PG_ScrollBar::VERTICAL);
	my_colorslider->SetRange(0, 255);

	my_colorslider->SetBackground((SDL_Surface*)NULL);
	my_colorslider->SetSimpleBackground(false);
	my_colorslider->sigSlide.connect(sigc::mem_fun(*this, &PG_ColorSelector::handle_colorslide));
	
	my_colorresult = new PG_ThemeWidget(this, PG_Rect(r.h+20,r.h/2,r.w - ((r.h+20)+5), r.h - (5+r.h/2)));
	my_colorresult->SetSimpleBackground(true);
	
	SetBaseColor(my_colorbox->GetBaseColor());
}

PG_ColorSelector::~PG_ColorSelector() {
}

void PG_ColorSelector::SetColor(const PG_Color& c) {
	my_color = c;
}

void PG_ColorSelector::SetBaseColor(const PG_Color& c) {
	PG_Gradient g;
	my_basecolor = c;

	g.colors[0].r = my_basecolor.r;
	g.colors[0].g = my_basecolor.g;
	g.colors[0].b = my_basecolor.b;

	g.colors[1].r = my_basecolor.r;
	g.colors[1].g = my_basecolor.g;
	g.colors[1].b = my_basecolor.b;
	
	g.colors[2].r = 0;
	g.colors[2].g = 0;
	g.colors[2].b = 0;
	
	g.colors[3].r = 0;
	g.colors[3].g = 0;
	g.colors[3].b = 0;

	my_colorslider->SetGradient(g);
	
	float v = my_colorslider->GetPosition();
	
	PG_Color r;
	r.r = (Uint8)(((float)c.r / 255.0) * (255.0 - v));
	r.g = (Uint8)(((float)c.g / 255.0) * (255.0 - v));
	r.b = (Uint8)(((float)c.b / 255.0) * (255.0 - v));
	
	my_colorresult->SetBackgroundColor(r);
	my_colorresult->Update();
}

bool PG_ColorSelector::handle_colorslide(const PG_ScrollBar* scrollbar, long data) {
	SetBaseColor(my_colorbox->GetBaseColor());
	return true;
}
