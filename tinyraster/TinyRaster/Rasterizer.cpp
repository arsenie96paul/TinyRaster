/*---------------------------------------------------------------------
*
* Copyright © 2016  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <algorithm>
#include <math.h>

#include "Rasterizer.h"

Rasterizer::Rasterizer(void)
{
	mFramebuffer = NULL;
	mScanlineLUT = NULL;
}

void Rasterizer::ClearScanlineLUT()
{
	Scanline *pScanline = mScanlineLUT;

	for (int y = 0; y < mHeight; y++)
	{
		(pScanline + y)->clear();
		(pScanline + y)->shrink_to_fit();
	}
}

unsigned int Rasterizer::ComputeOutCode(const Vector2 & p, const ClipRect& clipRect)
{
	unsigned int CENTRE = 0x0;
	unsigned int LEFT = 0x1;
	unsigned int RIGHT = 0x1 << 1;
	unsigned int BOTTOM = 0x1 << 2;
	unsigned int TOP = 0x1 << 3;
	unsigned int outcode = CENTRE;

	if (p[0] < clipRect.left)
		outcode |= LEFT;
	else if (p[0] >= clipRect.right)
		outcode |= RIGHT;

	if (p[1] < clipRect.bottom)
		outcode |= BOTTOM;
	else if (p[1] >= clipRect.top)
		outcode |= TOP;

	return outcode;
}

bool Rasterizer::ClipLine(const Vertex2d & v1, const Vertex2d & v2, const ClipRect& clipRect, Vector2 & outP1, Vector2 & outP2)
{
	//TODO: EXTRA This is not directly prescribed as an assignment exercise. 
	//However, if you want to create an efficient and robust rasteriser, clipping is a usefull addition.
	//The following code is the starting point of the Cohen-Sutherland clipping algorithm.
	//If you complete its implementation, you can test it by calling prior to calling any DrawLine2D .

	const Vector2 p1 = v1.position;
	const Vector2 p2 = v2.position;
	unsigned int outcode1 = ComputeOutCode(p1, clipRect);
	unsigned int outcode2 = ComputeOutCode(p2, clipRect);

	outP1 = p1;
	outP2 = p2;

	bool draw = false;

	return true;
}

void Rasterizer::WriteRGBAToFramebuffer(int x, int y, const Colour4 & colour)
{
	if ((x >= 0 && x < mWidth) && (y >= 0 && y < mHeight)) // exercise 1
	{
		PixelRGBA *pixel = mFramebuffer->GetBuffer();
		pixel[y*mWidth + x] = colour;
	}
}

Rasterizer::Rasterizer(int width, int height)
{
	//Initialise the rasterizer to its initial state
	mFramebuffer = new Framebuffer(width, height);
	mScanlineLUT = new Scanline[height];
	mWidth = width;
	mHeight = height;

	mBGColour.SetVector(0.0, 0.0, 0.0, 1.0);	//default bg colour is black
	mFGColour.SetVector(1.0, 1.0, 1.0, 1.0);    //default fg colour is white

	mGeometryMode = LINE;
	mFillMode = UNFILLED;
	mBlendMode = NO_BLEND;

	SetClipRectangle(0, mWidth, 0, mHeight);
}

Rasterizer::~Rasterizer()
{
	delete mFramebuffer;
	delete[] mScanlineLUT;
}

void Rasterizer::Clear(const Colour4& colour)
{
	PixelRGBA *pixel = mFramebuffer->GetBuffer();

	SetBGColour(colour);

	int size = mWidth*mHeight;

	for (int i = 0; i < size; i++)
	{
		//fill all pixels in the framebuffer with background colour
		*(pixel + i) = mBGColour;
	}
}

void Rasterizer::DrawPoint2D(const Vector2& pt, int size)
{
	int x = pt[0];
	int y = pt[1];

	WriteRGBAToFramebuffer(x, y, mFGColour);
}

void Rasterizer::DrawLine2D(const Vertex2d & v1, const Vertex2d & v2, int thickness)
{
	//The following code is basic Bresenham's line drawing algorithm.
	//The current implementation is only capable of rasterise a line in the first octant, where dy < dx and dy/dx >= 0
	//See if you want to read ahead https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html

	//TODO:
	//Ex 1.1 Complete the implementation of Rasterizer::DrawLine2D method. 
	//This method currently consists of a partially implemented Bresenham algorithm.
	//You must extend its implementation so that it is capable of drawing 2D lines with arbitrary gradient(slope).
	//Use Test 1 (Press F1) to test your implementation

	//Ex 1.2 Extend the implementation of Rasterizer::DrawLine2D so that it is capable of drawing lines based on a given thickness.
	//Note: The thickness is passed as an argument int thickness.
	//Use Test 2 (Press F2) to test your implementation

	//Ex 1.3 Extend the implementation of Rasterizer::DrawLine2D so that it is capable of interpolating colour across a line when each end-point has different colours.
	//Note: The variable mFillMode indicates if the fill mode is set to INTERPOLATED_FILL. 
	//The colour of each point should be linearly interpolated using the colours of v1 and v2.
	//Use Test 2 (Press F2) to test your implementation

	Vector2 pt1 = v1.position;
	Vector2 pt2 = v2.position;	

	// bools variables
	bool swap_vertices = false;
	bool negative_slope = false;
	bool swap_xy = false;

	// swap vertices ( case 1 ); set it true if x1 is greater then x2
	if (v1.position[0] > v2.position[0])
	{
		swap_vertices = true;
	}
	// if swap vertices is true, then change the points possition
	if (swap_vertices)
	{
		pt1 = v2.position;
		pt2 = v1.position;
	}

	int dx = pt2[0] - pt1[0];
	int dy = pt2[1] - pt1[1];
	int mdx = abs(dx);
	int mdy = abs(dy);

	// negative slope ( case 2 ); set it true if dy is less then 0
	if ((pt2[1] - pt1[1])< 0.0f)
	{
		negative_slope = true;
	}
	//  if negative slope is true, change y1 with -y1 and dy with -dy
	if (negative_slope)
	{
		pt1[1] = -pt1[1];
		dy = -dy;
	}
	// swap x and y ( case 3 ); set it true if dx is less then dy
	if (dx < dy)
	{
		swap_xy = true;
	}
	int epsilon = 0;

	int x = pt1[0];
	int y = pt1[1];
	int ex = pt2[0];
	int ey = pt2[1]; // added

	// swap x and y ( case 3 ) while negative slope is flase
	if (swap_xy && !negative_slope)
	{
		int temp; // temporary variable used to change dx and dy

		//change x with y and y with x
		x = pt1[1];
		y = pt1[0];

		//change dx with dy and dy with dx using temporary variable
		temp = dx;
		dx = dy;
		dy = temp;

		//set the value of x2 to be y2
		ex = pt2[1];
	}
	// swap x and y ( case 3 ) while negative slope is true
	if (swap_xy && negative_slope)
	{
		int temp;// temporary variable used to change dx and dy

		//change x with y and y with x
		x = pt1[1];
		y = pt1[0];

		//change dx with dy and dy with dx
		temp = dx;
		dx = dy;
		dy = temp;
		
		//set the value of x2 to be -y2
		ex = -pt2[1];
	}
	while (x <= ex)
	{

		Vector2 temp(x, y);
		// negative slope ( change y ; case 2 )
		if (negative_slope)
		{
			temp[1] = -y;
		}
		//(swap x y (case 3 ) with negative slope flase
		if (swap_xy && !negative_slope)
		{
			temp[0] = y;
			temp[1] = x;
		}
		//(swap x y (case 3 ) with negative slope true
		if (swap_xy && negative_slope)
		{
			temp[0] = y;
			temp[1] = -x;
		}
		// THICKNESS
		int vartemp = 0; // temporary variable used to count from 0 to thickness
		do
		{
			//thickness vectors			
			Vector2 temp_under(x, y);
			Vector2 temp_above(x, y);	

			// octans cases used for thickness
			if (!swap_xy && negative_slope)
			{
				temp_under[1] = -y - vartemp;
				temp_above[1] = -y + vartemp;
			}
			if (swap_xy && !negative_slope)
			{
				temp_under[0] = y - vartemp;
				temp_under[1] = x;

				temp_above[0] = y + vartemp;
				temp_above[1] = x;
			}

			if (swap_xy && negative_slope)
			{
				temp_under[0] = y - vartemp;
				temp_under[1] = -x;

				temp_above[0] = y + vartemp;
				temp_above[1] = -x;
			}
			if (!swap_xy && !negative_slope)
			{
				temp_under[0] = x;
				temp_under[1] = y - vartemp;

				temp_above[0] = x;
				temp_above[1] = y + vartemp;
			}
			vartemp++;
			DrawPoint2D(temp_under);
			DrawPoint2D(temp_above);
		} while (vartemp < thickness); // do while loop used to draw all the lines between line and thickness
		
		Colour4 colour = v1.colour;
		//INTERPOLATE COLOUR
		if (mFillMode == INTERPOLATED_FILLED) // if fill mode is interpolated, then interpolate the colours
		{
			double t; 
			for (int i = 0; i < 3; i++)
			{
				// using swap_xy I succeed to interpolate colours all around the x and y axes
				if (!swap_xy) 
				{
					t = abs((x - pt1[0]) / (pt2[0] - pt1[0]));					
				}
				if (swap_xy)
				{
					t = abs((x - pt1[1]) / (pt2[1] - pt1[1]));					
				}
				colour[i] = t*v2.colour[i] + (1 - t)*v1.colour[i];
			}
		}
		SetFGColour(colour);
		DrawPoint2D(temp);
		epsilon += dy;

		if ((epsilon << 1) >= dx)
		{
			y++;

			epsilon -= dx;
		}

		x++;
	}
}

void Rasterizer::DrawUnfilledPolygon2D(const Vertex2d * vertices, int count)
{
	//TODO:
	//Ex 2.1 Implement the Rasterizer::DrawUnfilledPolygon2D method so that it is capable of drawing an unfilled polygon, i.e. only the edges of a polygon are rasterised. 
	//Please note, in order to complete this exercise, you must first complete Ex1.1 since DrawLine2D method is reusable here.
	//Note: The edges of a given polygon can be found by conntecting two adjacent vertices in the vertices array.
	//Use Test 3 (Press F3) to test your solution.

	int vartemp = 0; // temporary variable used to go by all vertices
	while (vartemp < count - 1)
	{
		DrawLine2D(vertices[vartemp], vertices[vartemp + 1]);
		vartemp = vartemp + 1;
	}
	DrawLine2D(vertices[count - 1], vertices[0]); // draw line between last vertices and first one
}

void Rasterizer::ScanlineFillPolygon2D(const Vertex2d * vertices, int count)
{
	//TODO:
	//Ex 2.2 Implement the Rasterizer::ScanlineFillPolygon2D method method so that it is capable of drawing a solidly filled polygon.
	//Note: You can implement floodfill for this exercise however scanline fill is considered a more efficient and robust solution.
	//		You should be able to reuse DrawUnfilledPolygon2D here.
	//
	//Use Test 4 (Press F4) to test your solution, this is a simple test case as all polygons are convex.
	//Use Test 5 (Press F5) to test your solution, this is a complex test case with one non-convex polygon.

	//Ex 2.3 Extend Rasterizer::ScanlineFillPolygon2D method so that it is capable of alpha blending, i.e. draw translucent polygons.
	//Note: The variable mBlendMode indicates if the blend mode is set to alpha blending.
	//To do alpha blending during filling, the new colour of a point should be combined with the existing colour in the framebuffer using the alpha value.
	//Use Test 6 (Press F6) to test your solution

	int vartemp = 0;
	while (vartemp < count - 1)
	{
		DrawLine2D(vertices[vartemp], vertices[vartemp + 1]);
		vartemp = vartemp + 1;
	}
	DrawLine2D(vertices[count - 1], vertices[0]);
}

void Rasterizer::ScanlineInterpolatedFillPolygon2D(const Vertex2d * vertices, int count)
{
	//TODO:
	//Ex 2.4 Implement Rasterizer::ScanlineInterpolatedFillPolygon2D method so that it is capable of performing interpolated filling.
	//Note: mFillMode is set to INTERPOLATED_FILL
	//		This exercise will be more straightfoward if Ex 1.3 has been implemented in DrawLine2D
	//Use Test 7 to test your solution

}

void Rasterizer::DrawCircle2D(const Circle2D & inCircle, bool filled)
{
	//TODO:
	//Ex 2.5 Implement Rasterizer::DrawCircle2D method so that it can draw a filled circle.
	//Note: For a simple solution, you can first attempt to draw an unfilled circle in the same way as drawing an unfilled polygon.
	//Use Test 8 to test your solution
	
	Vertex2d v1 = { inCircle.colour}; // point one, taking the circle colour
	Vertex2d v2 = { inCircle.colour}; // point two, taking the circle colour
	Vertex2d center = { inCircle.colour, inCircle.centre }; // point taking the coordinates and colour of the circle

	int nsegments = 16;
	float pi = 3.14159265359;
	float t = 0;
	float dt = (2 * pi) / nsegments;
	
	while (t < 360.0f) // one while, used to go all around the circle
	{
		// coordinates of first point
		v1.position[0] = inCircle.radius*cos(t) + inCircle.centre[0];
		v1.position[1] = inCircle.radius*sin(t) + inCircle.centre[1];
		// coordinates of second point
		v2.position[0] = inCircle.radius*cos(t - dt) + inCircle.centre[0];
		v2.position[1] = inCircle.radius*sin(t - dt) + inCircle.centre[1];
		// I used one if statement to see if the circle should be filled or not
		if (filled == true)
		{
			DrawLine2D(v1, v2); // drawing circle
			DrawLine2D(v1, center, 4); // filling it, drawing lines from the middle to the points and using thickness to full fill it
		}
		else
		{
			DrawLine2D(v1, v2); // drawing circle
		}

		t = t + 1; 
	}
}

Framebuffer *Rasterizer::GetFrameBuffer() const
{
	return mFramebuffer;
}
