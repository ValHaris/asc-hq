/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cmath>
#include "geometry.h"

Point2D::Point2D(): xCoord(0), yCoord(0) {}

Point2D::Point2D(int x, int y): xCoord(x), yCoord(y) {}

Point2D::~Point2D() {}

void Point2D::move(int width, int height) {
  xCoord+=width;
  yCoord+=height;
}



void Point2D::set(int x, int y) {
  xCoord = x;
  yCoord = y;
}

void Point2D::move(const Vector2D& v) {
  xCoord+=v.getXComponent();
  yCoord+=v.getYComponent();
}

//***************************************************************************************************************************************
Vector2D::Vector2D():xComponent(0), yComponent(0) {}

Vector2D::Vector2D(int x, int y):xComponent(x), yComponent(y) {}

Vector2D::~Vector2D() {}

int Vector2D::getXComponent() const {
  return xComponent;
}

int Vector2D::getYComponent() const {
  return yComponent;
}
double Vector2D::getLength() const{
  return std::sqrt(std::pow(double(xComponent), 2) + std::pow(double(yComponent),2));
}

bool Vector2D::isZeroVector() const {
  return ((xComponent == 0) && (yComponent == 0));
}


