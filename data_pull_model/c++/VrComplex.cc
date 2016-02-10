/* -*- Mode: c++ -*-
 *
 *  Copyright 1997 Massachusetts Institute of Technology
 * 
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation, and that the name of M.I.T. not be used in advertising or
 *  publicity pertaining to distribution of the software without specific,
 *  written prior permission.  M.I.T. makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as is"
 *  without express or implied warranty.
 * 
 */
// Based on the C++ implementation
// Written by Jason Merrill based upon the specification in the 27 May 1994
// C++ working paper, ANSI document X3J16/94-0098.

#include <VrComplex.h>

VrComplex
cos (const VrComplex& x)
{
  return VrComplex (cos (real (x)) * cosh (imag (x)),
			   - sin (real (x)) * sinh (imag (x)));
}

VrComplex
cosh (const VrComplex& x)
{
  return VrComplex (cosh (real (x)) * cos (imag (x)),
			   sinh (real (x)) * sin (imag (x)));
}

VrComplex
exp (const VrComplex& x)
{
  return polar (float (exp (real (x))), imag (x));
}

VrComplex
log (const VrComplex& x)
{
  return VrComplex (log (abs (x)), arg (x));
}

VrComplex
pow (const VrComplex& x, const VrComplex& y)
{
  float logr = log (abs (x));
  float t = arg (x);

  return polar (float (exp (logr * real (y) - imag (y) * t)),
		float (imag (y) * logr + real (y) * t));
}

VrComplex
pow (const VrComplex& x, float y)
{
  return exp (float (y) * log (x));
}

VrComplex
pow (float x, const VrComplex& y)
{
  return exp (y * float (log (x)));
}

VrComplex
sin (const VrComplex& x)
{
  return VrComplex (sin (real (x)) * cosh (imag (x)),
			   cos (real (x)) * sinh (imag (x)));
}

VrComplex
sinh (const VrComplex& x)
{
  return VrComplex (sinh (real (x)) * cos (imag (x)),
			   cosh (real (x)) * sin (imag (x)));
}

#include <iostream.h>

istream&
operator >> (istream& is, VrComplex& x)
{
  float re, im = 0;
  char ch = 0;

  if (is.ipfx0 ())
    {
      if (is.peek () == '(')
	is >> ch;
      is >> re;
      if (ch == '(')
	{
	  is >> ch;
	  if (ch == ',')
	    is >> im >> ch;
	}
    }
  is.isfx ();

  if (ch != 0 && ch != ')')
    is.setstate (ios::failbit);
  else if (is.good ())
    x = VrComplex (re, im);

  return is;
}

ostream&
operator << (ostream& os, const VrComplex& x)
{
  return os << '(' << real (x) << ',' << imag (x) << ')';
}

// The code below is adapted from f2c's libF77, and is subject to this
// copyright:

/****************************************************************
Copyright 1990, 1991, 1992, 1993 by AT&T Bell Laboratories and Bellcore.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the names of AT&T Bell Laboratories or
Bellcore or any of their entities not be used in advertising or
publicity pertaining to distribution of the software without
specific, written prior permission.

AT&T and Bellcore disclaim all warranties with regard to this
software, including all implied warranties of merchantability
and fitness.  In no event shall AT&T or Bellcore be liable for
any special, indirect or consequential damages or any damages
whatsoever resulting from loss of use, data or profits, whether
in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of
this software.
****************************************************************/

VrComplex& VrComplex::
operator /= (const VrComplex& y)
{
  float ar = abs (y.re);
  float ai = abs (y.im);
  float nr, ni;
  float t, d;
  if (ar <= ai)
    {
      t = y.re / y.im;
      d = y.im * (1 + t*t);
      nr = (re * t + im) / d;
      ni = (im * t - re) / d;
    }
  else
    {
      t = y.im / y.re;
      d = y.re * (1 + t*t);
      nr = (re + im * t) / d;
      ni = (im - re * t) / d;
    }
  re = nr;
  im = ni;
  return *this;
}

VrComplex
operator / (const VrComplex& x, const VrComplex& y)
{
  float ar = abs (real (y));
  float ai = abs (imag (y));
  float nr, ni;
  float t, d;
  if (ar <= ai)
    {
      t = real (y) / imag (y);
      d = imag (y) * (1 + t*t);
      nr = (real (x) * t + imag (x)) / d;
      ni = (imag (x) * t - real (x)) / d;
    }
  else
    {
      t = imag (y) / real (y);
      d = real (y) * (1 + t*t);
      nr = (real (x) + imag (x) * t) / d;
      ni = (imag (x) - real (x) * t) / d;
    }
  return VrComplex (nr, ni);
}

VrComplex
operator / (float x, const VrComplex& y)
{
  float ar = abs (real (y));
  float ai = abs (imag (y));
  float nr, ni;
  float t, d;
  if (ar <= ai)
    {
      t = real (y) / imag (y);
      d = imag (y) * (1 + t*t);
      nr = x * t / d;
      ni = -x / d;
    }
  else
    {
      t = imag (y) / real (y);
      d = real (y) * (1 + t*t);
      nr = x / d;
      ni = -x * t / d;
    }
  return VrComplex (nr, ni);
}

VrComplex
pow (const VrComplex& xin, int y)
{
  if (y == 0)
    return VrComplex (1.0);
  VrComplex r (1.0);
  VrComplex x (xin);
  if (y < 0)
    {
      y = -y;
      x = 1/x;
    }
  for (;;)
    {
      if (y & 1)
	r *= x;
      if (y >>= 1)
	x *= x;
      else
	return r;
    }
}

VrComplex
sqrt (const VrComplex& x)
{
  float r = abs (x);
  float nr, ni;
  if (r == 0.0)
    nr = ni = r;
  else if (real (x) > 0)
    {
      nr = sqrt (0.5 * (r + real (x)));
      ni = imag (x) / nr / 2;
    }
  else
    {
      ni = sqrt (0.5 * (r - real (x)));
      if (imag (x) < 0)
	ni = - ni;
      nr = imag (x) / ni / 2;
    }
  return VrComplex (nr, ni); 
}

