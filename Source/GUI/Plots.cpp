/*  Copyright (c) BAVC. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------

#include "GUI/Plots.h"
#include "GUI/Plot.h"
#include "Core/Core.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScrollArea>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <cmath>
//---------------------------------------------------------------------------

class XAxisFormatBox: public QComboBox
{
public:
    XAxisFormatBox( QWidget* parent = NULL ):
        QComboBox( parent )
    {
        setContentsMargins( 0, 0, 0, 0 );

        addItem( "Frames" );
        addItem( "Seconds" );
        addItem( "Minutes" );
        addItem( "Hours" );
        addItem( "Time" );
    }
};

class ScaleWidget: public QwtScaleWidget
{
public:
    ScaleWidget( QWidget* parent = NULL ):
        QwtScaleWidget( QwtScaleDraw::BottomScale, parent )
    {
        ScaleDraw* sd = new ScaleDraw();
        sd->setFormat( Plots::AxisTime );
        setScaleDraw( new ScaleDraw() );
    }

	void setScale( double from, double to )
	{
		QwtLinearScaleEngine se;
		setScaleDiv( se.divideScale( from, to, 5, 8 ) );
	}

    void setFormat( int format )
    {
        ScaleDraw* sd = dynamic_cast<ScaleDraw*>( scaleDraw() );
        if ( sd )
            sd->setFormat( format );
    }

    int format() const
	{
        const ScaleDraw* sd = dynamic_cast<const ScaleDraw*>( scaleDraw() );
		return sd ? sd->format() : 0;
	}

	QwtScaleDiv scaleDiv() const
	{
		return scaleDraw()->scaleDiv();
	}

	QwtInterval interval() const
	{
		return scaleDraw()->scaleDiv().interval();
	}

private:
    class ScaleDraw: public QwtScaleDraw
    {
    public:
        void setFormat( int format )
        {
            if ( format != m_format )
            {
                m_format = format;
                invalidateCache();
            }
        }

		int format() const
		{
			return m_format;
		}

        virtual QwtText label( double value ) const
        {
            if ( m_format == Plots::AxisTime )
            {
                const int h = static_cast<int>( value / 3600 );
                const int m = static_cast<int>( value / 60 );
                const int s = static_cast<int>( value );

                QString label;

                if ( scaleDiv().interval().width() > 10.0 )
                {
                    label.sprintf( "%02d:%02d:%02d", 
                        h, m - h * 60, s - m * 60 );
                }
                else
                {
                    const int ms = qRound( ( value - s ) * 1000.0 );
                    label.sprintf( "%02d:%02d:%02d.%03d", 
                        h, m - h * 60, s - m * 60, ms);
                }

                return label;
            }

            return QwtScaleDraw::label( value );
        }

    private:
        int m_format;
    };
};

class PlotLegend: public QwtLegend
{
public:
    PlotLegend( QWidget *parent ):
        QwtLegend( parent )
    {
        setMinimumHeight( 1 );
        setMaxColumns( 1 );
        setContentsMargins( 0, 0, 0, 0 );

        QLayout* layout = contentsWidget()->layout();
        layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
        layout->setSpacing( 0 );

        QScrollArea *scrollArea = findChild<QScrollArea *>();
        if ( scrollArea )
        {
            scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        }

#if 1
        QFont fnt = font();
        if ( fnt.pointSize() > 8 )
        {
            fnt.setPointSize( 8 );
            setFont( fnt );
        }
#endif
    }

protected:
    virtual QWidget *createWidget( const QwtLegendData &data ) const
    {
        QWidget *w = QwtLegend::createWidget( data );

        QwtLegendLabel *label = dynamic_cast<QwtLegendLabel *>( w );
        if ( label )
        {
            label->setMargin( 0 );
        }

        return w;
    }
};

//---------------------------------------------------------------------------
Plots::Plots( QWidget *parent, FileInformation* FileInformationData_ ) :
    QWidget( parent ),
    m_fileInfoData( FileInformationData_ ),
    m_zoomLevel( 1 ),
    m_dataTypeIndex( Plots::AxisSeconds )
{
    QGridLayout* layout = new QGridLayout( this );
    layout->setSpacing( 1 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    for ( int row = 0; row < PlotType_Max; row++ )
    {
		Plot* plot = new Plot( ( PlotType )row, this );

		// we allow to shrink the plot below height of the size hint
		plot->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding );
        plot->setAxisScale( QwtPlot::xBottom, 0, videoStats()->x_Max[m_dataTypeIndex] );

		QwtLegend *legend = new PlotLegend( this );
		connect( plot, SIGNAL( legendDataChanged( const QVariant &, const QList<QwtLegendData> & ) ),
				 legend, SLOT( updateLegend( const QVariant &, const QList<QwtLegendData> & ) ) );

		connect( plot, SIGNAL( cursorMoved( double ) ), SLOT( onCursorMoved( double ) ) );
		plot->updateLegend();


        layout->addWidget( plot, row, 0 );
		layout->addWidget( legend, row, 1 );

		m_plots[row] = plot;
    }

	// bottom scale
	m_scaleWidget = new ScaleWidget();
    m_scaleWidget->setFormat( Plots::AxisTime );
	m_scaleWidget->setScale( 0, videoStats()->x_Max[m_dataTypeIndex] );
	layout->addWidget( m_scaleWidget, PlotType_Max, 0, 1, 2 );

    // combo box for the axis format
    XAxisFormatBox* xAxisBox = new XAxisFormatBox();
    xAxisBox->setCurrentIndex( Plots::AxisTime );
    connect( xAxisBox, SIGNAL( currentIndexChanged( int ) ),
        this, SLOT( onXAxisFormatChanged( int ) ) );

	int axisBoxRow = layout->rowCount() - 1;
#if 1
    // one row below to have space enough for bottom scale tick labels
    layout->addWidget( xAxisBox, PlotType_Max + 1, 1 );
#else
    layout->addWidget( xAxisBox, PlotType_Max, 1 );
#endif

    layout->setColumnStretch( 0, 10 );
    layout->setColumnStretch( 1, 0 );

    for ( int i = 0; i < PlotType_Max; i++ )
    {
        if ( m_plots[i]->isVisibleTo( this ) )
            syncPlot( ( PlotType )i );
    }

    setCursorPos( videoStats()->x[m_dataTypeIndex][framePos()] );
}

//---------------------------------------------------------------------------
Plots::~Plots()
{
}

//---------------------------------------------------------------------------
const QwtPlot* Plots::plot( PlotType Type ) const
{
    return m_plots[Type];
}

//---------------------------------------------------------------------------
Plot* Plots::plotAt( int row )
{
    return dynamic_cast<Plot*>( m_plots[row] );
}

//---------------------------------------------------------------------------
const Plot* Plots::plotAt( int row ) const
{
    return dynamic_cast<Plot*>( m_plots[row] );
}

void Plots::scrollXAxis()
{
    // position of the current frame has changed 

    if ( isZoomed() )
    {
    	// Put the current frame in center
    	const size_t pos = framePos();
        const size_t numFrames = visibleFramesCount();

        size_t Begin = 0;
        if ( pos > numFrames / 2 )
        {
            Begin = pos - numFrames / 2;
            if ( Begin + numFrames > ( videoStats()->x_Current_Max - 1 ) )
                Begin = ( videoStats()->x_Current_Max - 1 ) - numFrames;
        }

    	if ( Begin + numFrames > ( videoStats()->x_Current_Max - 1 ) )
        	Begin = ( videoStats()->x_Current_Max - 1 ) - numFrames;

    	const double x = videoStats()->x[m_dataTypeIndex][Begin];
		const double w = m_scaleWidget->interval().width();

    	m_scaleWidget->setScale( x, x + w );

        replotAll();
    }

    syncMarker();
}

//---------------------------------------------------------------------------
void Plots::syncMarker()
{
    setCursorPos( videoStats()->x[m_dataTypeIndex][framePos()] );
}

//---------------------------------------------------------------------------
void Plots::setCursorPos( double x )
{
    for ( int row = 0; row < PlotType_Max; ++row )
        plotAt( row )->setCursorPos( x );
}

//---------------------------------------------------------------------------
double Plots::axisStepSize( double s ) const
{
    for ( int d = 1; d <= 1000000; d *= 10 )
    {
        const double step = floor( s * d ) / d;
        if ( step > 0.0 )
            return step;
    }

    return 0.0;
}

//---------------------------------------------------------------------------
void Plots::syncPlot( PlotType Type )
{
    Plot* plot = plotAt( Type );
    if ( plot == NULL )
        return;

    VideoStats* video = videoStats();

    if ( PerPlotGroup[Type].Min != PerPlotGroup[Type].Max &&
            video->y_Max[Type] >= PerPlotGroup[Type].Max / 2 )
    {
        video->y_Max[Type] = PerPlotGroup[Type].Max;
    }

    const double yMax = video->y_Max[Type];
    if ( yMax )
    {
        if ( yMax > plot->axisInterval( QwtPlot::yLeft ).maxValue() )
        {
            const double stepCount = PerPlotGroup[Type].StepsCount;
            const double stepSize = axisStepSize( yMax / stepCount );

            if ( stepSize )
                plot->setAxisScale( QwtPlot::yLeft, 0, yMax, stepSize );
        }
    }
    else
    {
        //Special case, in order to force a scale of 0 to 1
        plot->setAxisScale( QwtPlot::yLeft, 0, 1, 1 );
    }

    for( unsigned j = 0; j < PerPlotGroup[Type].Count; ++j )
    {
        plot->setCurveSamples( j, video->x[m_dataTypeIndex],
            video->y[PerPlotGroup[Type].Start + j], video->x_Current );
    }
}

//---------------------------------------------------------------------------
void Plots::shiftXAxes()
{
    const size_t increment = zoomIncrement();

    int pos = framePos();
    if ( pos == -1 )
        return;

    if ( pos > increment / 2 )
        pos -= increment / 2;
    else
        pos = 0;

    if ( pos + increment > ( videoStats()->x_Current_Max - 1 ) )
        pos = ( videoStats()->x_Current_Max - 1 ) - increment;

    const double x = videoStats()->x[m_dataTypeIndex][pos];
    const double width = videoStats()->x_Max[m_dataTypeIndex] / m_zoomLevel;

	m_scaleWidget->setScale( x, x + width );
    replotAll();
}

//---------------------------------------------------------------------------
void Plots::Zoom_Move( size_t Begin )
{
    const double x0 = videoStats()->x[m_dataTypeIndex][Begin];
    const double width = m_scaleWidget->interval().width();

	double x = x0 + width; 
	if ( x > videoStats()->x_Current_Max )
		x = videoStats()->x_Current_Max;

	m_scaleWidget->setScale( x - width, x );

    replotAll();
}

//---------------------------------------------------------------------------
void Plots::alignYAxes()
{
    double maxExtent = 0;

    for ( int i = 0; i < PlotType_Max; i++ )
    {
        QwtScaleWidget *scaleWidget = m_plots[i]->axisWidget( QwtPlot::yLeft );

        QwtScaleDraw* scaleDraw = scaleWidget->scaleDraw();
        scaleDraw->setMinimumExtent( 0.0 );

        if ( m_plots[i]->isVisibleTo( this ) )
        {
            const double extent = scaleDraw->extent( scaleWidget->font() );
            maxExtent = qMax( extent, maxExtent );
        }
    }
    maxExtent += 3; // margin

    for ( int i = 0; i < PlotType_Max; i++ )
    {
        QwtScaleWidget *scaleWidget = m_plots[i]->axisWidget( QwtPlot::yLeft );
        scaleWidget->scaleDraw()->setMinimumExtent( maxExtent );
    }
}

//---------------------------------------------------------------------------
void Plots::onCursorMoved( double cursorX )
{
    const double* xData = videoStats()->x[m_dataTypeIndex];

    size_t pos = 0;
    while ( pos < videoStats()->x_Current_Max && cursorX >= xData[pos] )
        pos++;

    if ( pos )
    {
        if ( pos >= videoStats()->x_Current )
            pos = videoStats()->x_Current - 1;

        double Distance1 = cursorX - xData[pos - 1];
        double Distance2 = xData[pos] - cursorX;
        if ( Distance1 < Distance2 )
            pos--;
    }

    m_fileInfoData->Frames_Pos_Set( pos );
    setCursorPos( xData[pos] );
}

//---------------------------------------------------------------------------
void Plots::onXAxisFormatChanged( int format )
{
	if ( format == m_scaleWidget->format() )
		return;

	const int dataTypeIndex = m_dataTypeIndex;
	const int frame0 = visibleFramesBegin();
	const int numFrames = visibleFramesCount();
	
    m_scaleWidget->setFormat( format );

    if ( format == AxisTime )
        m_dataTypeIndex = AxisSeconds;
    else
        m_dataTypeIndex = format;

    for ( int i = 0; i < PlotType_Max; i++ )
    {
        if ( m_plots[i]->isVisibleTo( this ) )
            syncPlot( ( PlotType )i );
    }

	if ( m_dataTypeIndex != dataTypeIndex )
	{
		const double* x = videoStats()->x[m_dataTypeIndex];
    	m_scaleWidget->setScale( x[frame0], x[frame0 + numFrames - 1] );

		setCursorPos( x[framePos()] );
	}
	m_scaleWidget->update();
    replotAll();
}

//---------------------------------------------------------------------------
void Plots::setPlotVisible( PlotType Type, bool on )
{
	if ( on == m_plots[Type]->isVisibleTo( this ) )
		return;

    const int row = Type;

    QGridLayout* l = dynamic_cast<QGridLayout*>( layout() );

    if ( row < l->rowCount() )
    {
        for ( int col = 0; col < l->columnCount(); col++ )
        {
            QLayoutItem *item = l->itemAtPosition( row, col );
            if ( item && item->widget() )
                item->widget()->setVisible( on );
        }
    }

	alignYAxes();
}

//---------------------------------------------------------------------------
void Plots::zoomXAxis( bool up )
{
    if ( up )
    {
        m_zoomLevel *= 2;
    }
    else
    {
        if ( m_zoomLevel <= 1 )
			return;

        m_zoomLevel /= 2;
    }

	const double xMax = videoStats()->x_Current_Max;

    size_t pos = framePos();
    size_t Increment = xMax / m_zoomLevel;

    if ( pos + Increment / 2 > xMax )
        pos = xMax - Increment / 2;

    if ( pos > Increment / 2 )
        pos -= Increment / 2;
    else
        pos=0;

    if ( pos + Increment > ( videoStats()->x_Current_Max - 1 ) )
        pos = ( videoStats()->x_Current_Max - 1 ) - Increment;

    const double x = videoStats()->x[m_dataTypeIndex][pos];
    const double width = videoStats()->x_Max[m_dataTypeIndex] / m_zoomLevel;

    m_scaleWidget->setScale( x, x + width );

    replotAll();
}

//---------------------------------------------------------------------------
void Plots::replotAll()
{
    for ( int i = 0; i < PlotType_Max; i++ )
    {
		m_plots[i]->setAxisScaleDiv( QwtPlot::xBottom, m_scaleWidget->scaleDiv() );
        if ( m_plots[i]->isVisibleTo( this ) )
            m_plots[i]->replot();
    }
}

bool Plots::isZoomed() const
{
	return zoomLevel() > 1;
}

bool Plots::isZoomable() const
{
	return visibleFramesCount() > 4;
}

size_t Plots::visibleFramesCount() const
{
	// current scale width translated into frames
	const double w = m_scaleWidget->interval().width();
	return qRound( w * videoStats()->x_Current_Max / videoStats()->x_Max[m_dataTypeIndex] );
}

int Plots::visibleFramesBegin() const
{
	const double* x = videoStats()->x[m_dataTypeIndex];
	const double value = m_scaleWidget->interval().minValue();
#if 1
	// TODO
	int i;
	for ( i = 0; i < videoStats()->x_Current; i++ )
	{
		if ( x[i] > value )
			break;
	}
	return i - 1;
#endif
}

size_t Plots::zoomIncrement() const
{
	return videoStats()->x_Current_Max / m_zoomLevel;
}

int Plots::zoomLevel() const
{
	return qRound( videoStats()->x_Max[m_dataTypeIndex] / m_scaleWidget->interval().width() );
}
