/***************************************************************************
                          geometry.h  -  description
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
  


#ifndef GEOMETRY_H
#define GEOMETRY_H


/**
@brief A Vector2D is a line (without a defined start point) in a 2 dimensional space and is deccribed by its
       x- and y component
@author Kevin Hirschmann

*/
class Vector2D{
private:
int xComponent;
int yComponent;

public:
 /**
 @brief Default constructor 
 */
 Vector2D();
 /**
 @brief Constructor for creating a new vector
 @param x The xComponent
 @param y The yComponent
 */
 Vector2D(int x, int y);
 /**
 @brief Destructor
 */
 ~Vector2D();
 /**
 @brief Retrieves the xComponent of the Vector
 @return the xComponent 
 */
 int getXComponent() const;
 /**
 @brief Retrieves the yComponent of the Vector
 @return the yComponent
 */
 int getYComponent() const;
 /**
 @brief Calculates the length of the vector (Pythagoras)
 @return The length of the Vector
 */
 double getLength() const;
 /**
 @brief Checks if the vector is the zero-vector
 @return true if x- and yComponent == 0; flase otherwise
 */
 bool isZeroVector() const;
};

/**
@brief A point in a 2-dimensional space
@author Kevin Hirschmann
*/
class Point2D{
private:
int xCoord;
int yCoord;
public:
  /**
  @brief Default Constructor
  */
  Point2D(); 
  /**
  @brief Constructor for creating a new point
  @param x The x-Coordinate
  @param y The y-Coordinate
  */
  Point2D(int x, int y);
  /**
  @brief Destructor
  */
  ~Point2D();
  /**
  @brief Moves the Point x units horizontally and y units vertically
  @param x point is moved x units horziontally
  @param y point is moved y units vertically 
  */
  void move(int x, int y);
  /**
  @brief Moves the Point along a vector
  @param v vector describing movement
  */
  void move(const Vector2D& v);
  /**
  @brief Sets the point to new coordinates
  @param x The new x coordinate (horizontal)
  @param y the new y coordinate (vertical)
  */
  void set(int x, int y);
  /**
  @brief Gets the xCoordinate of the point
  @return xCoordinate
  */
  int getX() const{
    return xCoord;
  };
  /**
  @brief Gets the yCoordinate of the point
  @return yCoordinate
  */
  int getY() const{
    return yCoord;
  }
};

#endif

