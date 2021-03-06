#include "GCodeDebugView.h"
#include <iostream>
#include <cairo.h>
#include <cairo-svg.h>
#include <cmath>

using namespace std;

/** Debug graphic implementation */
struct GCodeDebugViewImpl : GCodeDebugView
{
    cairo_t *c;
    cairo_surface_t *cs;

    GCodeDebugViewImpl() :
        c(NULL),
        cs(NULL)
    {
    }

    virtual ~GCodeDebugViewImpl()
    {
        if (cs)
            cairo_surface_destroy (cs);
        if (c)
            cairo_destroy(c);
    }
    virtual void Segment(double x1,double y1,double x2,double y2,int nColor,double width);
    virtual void Point(double x,double y,int nColor);
    /** Scale size */
    static double Scale(double sz);
    /** Scale position and flip Y axis */
    static void Scale(double& x,double& y);
    virtual void Sequences(std::vector<std::pair<double,double>>& v,int nColor,double width);
    virtual void Array(double x1,double y1,double x2,double y2);
};

double GCodeDebugViewImpl::Scale(double sz)
{
    return sz * 60.0;
}

void GCodeDebugViewImpl::Scale(double& x,double& y)
{
    x = Scale(x);
    y = (200.0 - y);
    y = Scale(y);
}

void GCodeDebugViewImpl::Array(double x1,double y1,double x2,double y2)
{
    Scale(x1,y1);
    Scale(x2,y2);
    cairo_save(c);
    cairo_matrix_t matrix;
    cairo_matrix_init(&matrix,
            y2-y1,x1-x2,
            x2-x1,y2-y1,
            x1,y1);
    cairo_transform(c,&matrix);
    cairo_scale(c,1.0/8.0,1.0/8.0);


    cairo_move_to(c,-1,0);
    cairo_line_to(c,1,0);
    cairo_line_to(c,1,4);
    cairo_line_to(c,3,4);
    cairo_line_to(c,0,8);
    cairo_line_to(c,-3,4);
    cairo_line_to(c,-1,4);
    cairo_line_to(c,-1,0);

    cairo_set_source_rgba(c,0,1,0.3,0.6);
    cairo_fill(c);

    cairo_restore(c);
}

void GCodeDebugViewImpl::Sequences(std::vector<std::pair<double,double>>& v,int nColor,double width)
{
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    for (int i=0;i<v.size();i++)
    {
        double x1 = v[i].first;
        double y1 = v[i].second;
        Scale(x1,y1);
        cairo_set_line_width(c,Scale(width));
        switch (nColor)
        {
            case 0:
                cairo_set_source_rgba(c,1,0,0,0.9);
                break;
            case 1:
                cairo_set_source_rgba(c,1,0,0,0.1);
                break;
            case 3:
                cairo_set_source_rgba(c,1,0,0,0); // transparent
                break;
            case 2:
            default:
                cairo_set_source_rgba(c,0,1,1,0.4);
                break;
        }
        if (i == 0)
            cairo_move_to(c,x1,y1);
        else
            cairo_line_to(c,x1,y1);
    }
    cairo_stroke(c);
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(c,Scale(width)*0.5);
    for (int i=0;i<v.size();i++)
    {
        double x1 = v[i].first;
        double y1 = v[i].second;
        Scale(x1,y1);
        switch (nColor)
        {
            case 0:
            default:
                cairo_set_source_rgba(c,0,0,0,0.5);
                break;
        }
        if (i == 0)
            cairo_move_to(c,x1,y1);
        else
            cairo_line_to(c,x1,y1);
    }
    cairo_stroke(c);
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(c,Scale(width)*0.1);
    for (int i=0;i<v.size();i++)
    {
        double x1 = v[i].first;
        double y1 = v[i].second;
        Scale(x1,y1);
        switch (nColor)
        {
            case 0:
            default:
                cairo_set_source_rgba(c,0,0,0,0.5);
                break;
        }
        if (i == 0)
            cairo_move_to(c,x1,y1);
        else
            cairo_line_to(c,x1,y1);
    }
    cairo_stroke(c);
}

void GCodeDebugViewImpl::Segment(double x1,double y1,double x2,double y2,int nColor,double width)
{
    Scale(x1,y1);
    Scale(x2,y2);
    cairo_set_line_width(c,Scale(width));
    switch (nColor)
    {
        case 0:
            cairo_set_source_rgba(c,1,0,0,1);
            break;
        case 1:
            cairo_set_source_rgba(c,1,0,0,0.1);
            break;
        case 3:
            cairo_set_source_rgba(c,1,0,0,0); // transparent
            break;
        case 2:
        default:
            cairo_set_source_rgba(c,0,1,1,0.4);
            break;
    }
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_move_to(c,x1,y1);
    cairo_line_to(c,x2,y2);
    cairo_stroke(c);

    cairo_set_line_width(c,Scale(width)*0.5);
    switch (nColor)
    {
        case 0:
        default:
            cairo_set_source_rgba(c,0,0,0,0.5);
            break;
    }
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_move_to(c,x1,y1);
    cairo_line_to(c,x2,y2);
    cairo_stroke(c);
}

void GCodeDebugViewImpl::Point(double x,double y,int nColor)
{
    Scale(x,y);
    switch (nColor)
    {
        case 0:
        default:
            cairo_set_source_rgba(c,0,1,0,0.7);
            break;
        case 1:
            cairo_set_source_rgba(c,0,0,1,0.7);
            break;
        case 2:
            cairo_set_source_rgba(c,1,0,0.5,0.7);
            break;
        case 3:
            cairo_set_source_rgba(c,0,1,0.2,0.7);
            break;

    }
    cairo_move_to(c,x,y);
    if (nColor == 1 || nColor == 3)
        cairo_arc(c,x,y,Scale(0.35)*0.3,0,2*3.14159265358979323846);
    else
        cairo_arc(c,x,y,Scale(0.35)*0.2,0,2*3.14159265358979323846);
    cairo_fill(c);
/*
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(c,0.35*10.0);
    cairo_move_to (c, x*10.0, y*10.0);
    cairo_close_path (c);
    cairo_stroke (c);
    */
}

std::unique_ptr<GCodeDebugView> GCodeDebugViewFactory()
{
    unique_ptr<GCodeDebugViewImpl> ret(new GCodeDebugViewImpl());
    ret->cs = cairo_svg_surface_create("post_stretch.svg",ret->Scale(200.0),ret->Scale(200.0));
    ret->c = cairo_create (ret->cs);
    return ret;
}
