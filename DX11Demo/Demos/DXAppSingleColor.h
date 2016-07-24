#pragma once
#include "DXApp.h"

class DXAppSingleColor : public DXApp
{
public:
	DXAppSingleColor(HINSTANCE);
	~DXAppSingleColor();

	void Update(float) override;
	void Render(float) override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
};

