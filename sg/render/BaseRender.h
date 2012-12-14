/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_BASERENDER_H
#define GWEN_BASERENDER_H

#include "base/string16.h"
#include "sg/basic_geometric_types.h"

class Texture;

//namespace Gwen 
//{
	class Font;
	class WindowProvider;
	
	namespace Renderer
	{
		class Base;

		class Base
		{
			public:

				Base();
				virtual ~Base();

				virtual void Init(){};

				virtual void Begin(){};
				virtual void End(){};

				virtual void SetDrawColor( Color color ){};

				virtual void DrawFilledRect( Rect rect ){};;

				virtual void StartClip(){};
				virtual void EndClip(){};

				virtual void LoadTexture( Texture* pTexture ){};
				virtual void FreeTexture( Texture* pTexture ){};
				virtual void DrawTexturedRect( Texture* pTexture, Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f ){};
				virtual void DrawTexturedRectAlpha( Texture* pTexture, Rect pTargetRect, float alpha, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f ){};
				virtual void DrawMissingImage( Rect pTargetRect );
				virtual Color PixelColour( Texture* pTexture, unsigned int x, unsigned int y, const Color& col_default = Color( 255, 255, 255, 255 ) ){ return col_default; }


				virtual void LoadFont( Font* pFont ){};
				virtual void FreeFont( Font* pFont ){};
				virtual void RenderText( Font* pFont, Point pos, const string16& text );
				virtual Point MeasureText( Font* pFont, const string16& text );

				//
				// No need to implement these functions in your derived class, but if 
				// you can do them faster than the default implementation it's a good idea to.
				//
				virtual void DrawLinedRect( Rect rect );
				virtual void DrawPixel( int x, int y );
				virtual void DrawShavedCornerRect( Rect rect, bool bSlight = false );

			public:

				//
				// Translate a panel's local drawing coordinate
				//  into view space, taking Offset's into account.
				//
				void Translate( int& x, int& y );
				void Translate( Rect& rect );

				//
				// Set the rendering offset. You shouldn't have to 
				// touch these, ever.
				//
				void SetRenderOffset( const Point& offset ){ m_RenderOffset = offset; }
				void AddRenderOffset( const Rect& offset ){ m_RenderOffset.x += offset.x; m_RenderOffset.y += offset.y; }
				const Point& GetRenderOffset() const { return m_RenderOffset; }

			private:

				Point m_RenderOffset;

			public:

				void SetClipRegion( Rect rect );
				void AddClipRegion( Rect rect );
				bool ClipRegionVisible();
				const Rect& ClipRegion() const;

			private:

				Rect m_rectClipRegion;

			public:
				
				void SetScale( float fScale ){ m_fScale = fScale; }
				float Scale() const { return m_fScale; }

				float m_fScale;


			public:

				//
				// Self Initialization, shutdown
				//

				virtual bool InitializeContext( WindowProvider* pWindow ){ return false; }
				virtual bool ShutdownContext( WindowProvider* pWindow ){ return false; }
				virtual bool ResizedContext( WindowProvider* pWindow, int w, int h ){ return false; }

				virtual bool BeginContext( WindowProvider* pWindow){ return false; }
				virtual bool EndContext( WindowProvider* pWindow ){ return false; }
				virtual bool PresentContext( WindowProvider* pWindow ){ return false; }



		};
	}
//}
#endif
