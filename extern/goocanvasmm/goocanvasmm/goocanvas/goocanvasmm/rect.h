// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GOOCANVASMM_RECT_H
#define _GOOCANVASMM_RECT_H


#include <glibmm.h>

/* Copyright (C) 1998-2006 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <goocanvasmm/itemsimple.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GooCanvasRect GooCanvasRect;
typedef struct _GooCanvasRectClass GooCanvasRectClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Goocanvas
{ class Rect_Class; } // namespace Goocanvas
namespace Goocanvas
{


class Rect : public Goocanvas::ItemSimple
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Rect CppObjectType;
  typedef Rect_Class CppClassType;
  typedef GooCanvasRect BaseObjectType;
  typedef GooCanvasRectClass BaseClassType;

private:  friend class Rect_Class;
  static CppClassType rect_class_;

private:
  // noncopyable
  Rect(const Rect&);
  Rect& operator=(const Rect&);

protected:
  explicit Rect(const Glib::ConstructParams& construct_params);
  explicit Rect(GooCanvasRect* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~Rect();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GooCanvasRect*       gobj()       { return reinterpret_cast<GooCanvasRect*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GooCanvasRect* gobj() const { return reinterpret_cast<GooCanvasRect*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GooCanvasRect* gobj_copy();

private:


protected:

  explicit Rect(double x = 0.0, double y = 0.0, double width = 0.0, double height = 0.0);
  

public:

  
  static Glib::RefPtr<Rect> create(double x =  0.0, double y =  0.0, double width =  0.0, double height =  0.0);


  //We put these in item.hg instead.
  //See the comment there.
  //_WRAP_PROPERTY("x", double)
  //_WRAP_PROPERTY("y", double)
  //_WRAP_PROPERTY("width", double)
  //_WRAP_PROPERTY("height", double)
  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal radius to use for rounded corners.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_radius_x() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The horizontal radius to use for rounded corners.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_radius_x() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical radius to use for rounded corners.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_radius_y() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The vertical radius to use for rounded corners.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_radius_y() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

}


namespace Glib
{
  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Goocanvas::Rect
   */
  Glib::RefPtr<Goocanvas::Rect> wrap(GooCanvasRect* object, bool take_copy = false);
}


#endif /* _GOOCANVASMM_RECT_H */
