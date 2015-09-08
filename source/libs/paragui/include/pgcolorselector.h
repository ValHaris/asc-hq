#ifndef PG_COLORSELECTOR_H
#define PG_COLORSELECTOR_H

#include "pgthemewidget.h"
#include "pgslider.h"

class PG_ColorSelector : public PG_ThemeWidget {
protected:
	
	class PG_ColorBox : public PG_ThemeWidget {
	public:
		
		PG_ColorBox(PG_ColorSelector* parent, const PG_Rect& r);
		
		inline PG_ColorSelector* GetParent() {
			return static_cast<PG_ColorSelector*>(PG_ThemeWidget::GetParent());
		}
		
		PG_Color GetBaseColor();

	protected:
		
		void eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst);
		
		bool eventMouseMotion(const SDL_MouseMotionEvent* motion);
		bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);
		bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);
		
	private:
		
		bool my_btndown;
		PG_Point p;
	};
	
public:
	
	PG_ColorSelector(PG_Widget* parent, const PG_Rect&r, const std::string& style="colorselector");
	~PG_ColorSelector();

	void SetColor(const PG_Color& c);
	
	inline void SetColorGradient(PG_Gradient g) {
		my_colorbox->SetGradient(g);
	}

protected:
	
	bool handle_colorslide(long data);

	void SetBaseColor(const PG_Color& c);
	
	PG_ColorBox* my_colorbox;
	PG_Slider* my_colorslider;
	PG_ThemeWidget* my_colorresult;
	
	PG_Color my_color;
	PG_Color my_basecolor;
	
	friend class PG_ColorBox;
};

#endif // PG_COLORSELECTOR_H
