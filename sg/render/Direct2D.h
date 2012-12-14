/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_DIRECT2D_H
#define GWEN_RENDERERS_DIRECT2D_H

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <list>

#include "base/string16.h"
#include "sg/basic_geometric_types.h"
#include "sg/render/BaseRender.h"

class Font;
class Texture;

//namespace Gwen 
//{
	namespace Renderer 
	{

		class Direct2D : public Renderer::Base
		{
			public:

				Direct2D();
				Direct2D( ID2D1RenderTarget* pDevice, IDWriteFactory* pDWriteFactory, IWICImagingFactory* pWICFactory );
				~Direct2D();

				virtual void Begin();
				virtual void End();
				virtual void Release();

				virtual void SetDrawColor(Color color);

				virtual void DrawFilledRect( Rect rect );

				virtual void LoadFont( Font* pFont );
				virtual void FreeFont( Font* pFont );
				virtual void RenderText( Font* pFont, Point pos, const string16& text );
				virtual Point MeasureText( Font* pFont, const string16& text );

				virtual void DeviceLost();
				virtual void DeviceAcquired( ID2D1RenderTarget* pRT );

				void StartClip();
				void EndClip();

				void DrawTexturedRect( Texture* pTexture, Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
				void DrawTexturedRectAlpha( Texture* pTexture, Rect pTargetRect, float alpha, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
				void LoadTexture( Texture* pTexture );
				void FreeTexture( Texture* pTexture );
				Color PixelColour( Texture* pTexture, unsigned int x, unsigned int y, const Color& col_default );

				void DrawLinedRect( Rect rect );
				void DrawShavedCornerRect( Rect rect, bool bSlight = false );

			private:
				bool InternalCreateDeviceResources();
				void InternalReleaseDeviceResources();

				ID2D1Factory*	m_pD2DFactory;
				HWND			m_pHWND;

			private:
				bool InternalLoadTexture( Texture* pTexture );
				bool InternalLoadFont( Font* pFont );

				void InternalFreeFont( Font* pFont, bool bRemove = true );
				void InternalFreeTexture( Texture* pTexture, bool bRemove = true );

			private:

				IDWriteFactory*		m_pDWriteFactory;
				IWICImagingFactory*	m_pWICFactory;
				ID2D1RenderTarget*	m_pRT;

				ID2D1SolidColorBrush* m_pSolidColorBrush;

				D2D1::ColorF		m_Color;

        std::list<Texture*>	m_TextureList;
        std::list<Font*> m_FontList;

		};

	}
//}
#endif
