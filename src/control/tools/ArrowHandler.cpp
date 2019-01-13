#include "ArrowHandler.h"

#include "gui/XournalView.h"
#include "control/Control.h"
#include "undo/InsertUndoAction.h"
#include <cmath>

void ArrowHandler::snapToGrid(double& x, double& y)
{
    XOJ_CHECK_TYPE(ArrowHandler);

    if (!xournal->getControl()->getSettings()->isSnapGrid())
    {
        return;
    }

    /**
     * Snap points to a grid:
     * If x/y coordinates are under a certain tolerance,
     * fix the point to the grid intersection value
     */
    double gridSize = 14.17;
    double tolerance = gridSize/2.0;

    double xRem = fmod(x,gridSize);
    double yRem = fmod(y,gridSize);

    bool snapX = false;
    bool snapY = false;

    double tmpX = 0;
    double tmpY = 0;

    if (xRem < tolerance)
    {
        tmpX = x - xRem;
        snapX = true;
    }
    if (xRem > gridSize - tolerance )
    {
        tmpX = x + (gridSize - xRem);
        snapX = true;
    }
    if (yRem < tolerance)
    {
        tmpY = y - yRem;
        snapY = true;
    }
    if (yRem > gridSize - tolerance )
    {
        tmpY = y + (gridSize - yRem);
        snapY = true;
    }

    if (snapX && snapY)
    {
        x = tmpX;
        y = tmpY;
    }
}

ArrowHandler::ArrowHandler(XournalView* xournal, XojPageView* redrawable, PageRef page)
 : BaseStrokeHandler(xournal, redrawable, page)
{
	XOJ_INIT_TYPE(ArrowHandler);
}

ArrowHandler::~ArrowHandler()
{
	XOJ_CHECK_TYPE(ArrowHandler);

	XOJ_RELEASE_TYPE(ArrowHandler);
}

void ArrowHandler::drawShape(Point& c, bool shiftDown)
{
	int count = stroke->getPointCount();

    /**
     * Snap first point to grid (if enabled)
     */
    if (!shiftDown && xournal->getControl()->getSettings()->isSnapGrid())
    {
        Point firstPoint = stroke->getPoint(0);
        snapToGrid(firstPoint.x,firstPoint.y);
        stroke->setFirstPoint(firstPoint.x,firstPoint.y);
    }

	if (count < 1)
	{
		stroke->addPoint(c);
	}
	else 
	{
		Point p = stroke->getPoint(0);

		//disable this to see such a cool "serrate brush" effect
		if (count > 4)
		{
			// remove previous points
			stroke->deletePoint(4);
			stroke->deletePoint(3);
			stroke->deletePoint(2);
			stroke->deletePoint(1);
		}

		// We've now computed the line points for the arrow
		// so we just have to build the head

		// set up the size of the arrowhead to be 1/8 the length of arrow
		double dist = sqrt(pow(c.x - p.x, 2.0) + pow(c.y - p.y, 2.0));
		double arrowDist = dist/8;

		double angle = atan2((c.y - p.y), (c.x - p.x));
		// an appropriate delta is Pi/3 radians for an arrow shape
		double delta = M_PI / 6.0;
		
		if (shiftDown || !xournal->getControl()->getSettings()->isSnapRotation())
		{		
			stroke->addPoint(c);
			stroke->addPoint(Point(c.x - arrowDist * cos(angle + delta), c.y - arrowDist * sin(angle + delta)));
			stroke->addPoint(c);
			stroke->addPoint(Point(c.x - arrowDist * cos(angle - delta), c.y - arrowDist * sin(angle - delta)));
		}
		else
		{
            double epsilon = 0.2;
			if (std::abs(angle) < epsilon)
			{
				angle = 0;
				stroke->addPoint(Point(c.x, p.y));
				stroke->addPoint(Point(c.x - arrowDist * cos(angle + delta), p.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(c.x, p.y));
				stroke->addPoint(Point(c.x - arrowDist * cos(angle - delta), p.y - arrowDist * sin(angle - delta)));
			}		
			else if (std::abs(angle - M_PI / 4.0) < epsilon)
			{
				angle = M_PI / 4.0;
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x, dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x - arrowDist * cos(angle + delta), dist / sqrt(2.0) + p.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x, dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x - arrowDist * cos(angle - delta), dist / sqrt(2.0) + p.y - arrowDist * sin(angle - delta)));
			}
			else if (std::abs(angle - 3.0 * M_PI / 4.0) < epsilon)
			{
				angle = 3.0 * M_PI / 4.0;
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x, dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x - arrowDist * cos(angle + delta), dist / sqrt(2.0) + p.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x, dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x - arrowDist * cos(angle - delta), dist / sqrt(2.0) + p.y - arrowDist * sin(angle - delta)));
			}
			else if (std::abs(angle + M_PI / 4.0) < epsilon)
			{
				angle = M_PI / 4.0;
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x, -dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x - arrowDist * cos(angle + delta), -dist / sqrt(2.0) + p.y + arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x, -dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(dist / sqrt(2.0) + p.x - arrowDist * cos(angle - delta), -dist / sqrt(2.0) + p.y + arrowDist * sin(angle - delta)));
			}
			else if (std::abs(angle + 3.0 * M_PI / 4.0) < epsilon)
			{
				angle = 3.0 * M_PI / 4.0;
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x, -dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x - arrowDist * cos(angle + delta), -dist / sqrt(2.0) + p.y + arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x, -dist / sqrt(2.0) + p.y));
				stroke->addPoint(Point(-dist / sqrt(2.0) + p.x - arrowDist * cos(angle - delta), -dist / sqrt(2.0) + p.y + arrowDist * sin(angle - delta)));
			}
			else if (std::abs(std::abs(angle) - M_PI) < epsilon)
			{
				angle = - M_PI;
				stroke->addPoint(Point(c.x, p.y));
				stroke->addPoint(Point(c.x - arrowDist * cos(angle + delta), p.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(c.x, p.y));
				stroke->addPoint(Point(c.x - arrowDist * cos(angle - delta), p.y - arrowDist * sin(angle - delta)));
			}
			else if (std::abs(angle - M_PI / 2.0) < epsilon)
			{
				angle = M_PI / 2.0;
				stroke->addPoint(Point(p.x, c.y));
				stroke->addPoint(Point(p.x - arrowDist * cos(angle + delta), c.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(p.x, c.y));
				stroke->addPoint(Point(p.x - arrowDist * cos(angle - delta), c.y - arrowDist * sin(angle - delta)));
			}
			else if (std::abs(angle + M_PI / 2.0) < epsilon)
			{
				angle = - M_PI / 2.0;
				stroke->addPoint(Point(p.x, c.y));
				stroke->addPoint(Point(p.x - arrowDist * cos(angle + delta), c.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(Point(p.x, c.y));
				stroke->addPoint(Point(p.x - arrowDist * cos(angle - delta), c.y - arrowDist * sin(angle - delta)));
			}
			else
			{
				stroke->addPoint(c);
				stroke->addPoint(Point(c.x - arrowDist * cos(angle + delta), c.y - arrowDist * sin(angle + delta)));
				stroke->addPoint(c);
				stroke->addPoint(Point(c.x - arrowDist * cos(angle - delta), c.y - arrowDist * sin(angle - delta)));
			}
		}
	}
}
