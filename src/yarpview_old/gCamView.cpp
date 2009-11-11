// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Matteo Brunettini
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// gCamView.cpp : Defines the entry point for the application.
//
 
#include "gCamView.h" 

#include <yarp/os/Property.h> 
#include <yarp/os/Network.h> 

#include "YarpImage2Pixbuf.h"

// Image Receiver
extern YARPImgRecv *ptr_imgRecv;
// Image to Display
extern yarp::sig::ImageOf<yarp::sig::PixelRgb> *ptr_inputImg;
// Semaphore
extern yarp::os::Semaphore *ptr_semaphore;

static void createObjects() {
    ptr_imgRecv = new YARPImgRecv;
    ptr_inputImg = new yarp::sig::ImageOf<yarp::sig::PixelRgb>;
    ptr_semaphore = new yarp::os::Semaphore;
}

static void deleteObjects() {
    delete ptr_imgRecv;
    delete ptr_inputImg;
    delete ptr_semaphore;
}


//-------------------------------------------------
// Main Window Callbacks
//-------------------------------------------------

static gint timeout_CB (gpointer data)
{
	if ( (!_freezed) && getImage() )
        {
            //             int imageWidth, imageHeight, pixbufWidth, pixbufHeight;
            //             _semaphore.wait();
            //            imageWidth = _inputImg.width();
            //            imageHeight = _inputImg.height();
            //            _semaphore.post();
            //            pixbufWidth = gdk_pixbuf_get_width(frame);
            //            pixbufHeight = gdk_pixbuf_get_height(frame);
            //            if ( (imageWidth != pixbufWidth) || (imageHeight != pixbufHeight) )
            //                            {
            //                    g_object_unref(frame);
            //                    frame = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, imageWidth, imageHeight);
            //            }
            //            _frameN++;	
            gtk_widget_queue_draw (da);
            //            if (_savingSet)
            //                saveCurrentFrame();
        }

	return TRUE;
}


static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{
	// If you return FALSE in the "delete_event" signal handler,
    // GTK will emit the "destroy" signal. Returning TRUE means
    // you don't want the window to be destroyed.
    // This is useful for popping up 'are you sure you want to quit?'
    // type dialogs. 

	cleanExit();

	return TRUE;

}

static gint expose_CB (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	if ((frame) && (mainWindow))
        {
            guchar *pixels;
            unsigned int rowstride;
            unsigned int imageWidth, imageHeight, areaWidth, areaHeight;

            _semaphore.wait();
            yarpImage2Pixbuf(&_inputImg, frame);
            imageWidth = _inputImg.width();
            imageHeight = _inputImg.height();
            _semaphore.post();
            
            if (imageWidth==0||imageHeight==0) {
                return TRUE;
            }
 
            areaWidth = event->area.width;
            areaHeight = event->area.height;

            unsigned int pixbufWidth=gdk_pixbuf_get_width(frame);
            unsigned int pixbufHeight=gdk_pixbuf_get_height(frame);

            if ((imageWidth!=pixbufWidth) || (imageHeight!=pixbufHeight))
                {
                    g_object_unref(frame);
                    frame=gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, imageWidth, imageHeight);
                }

            if ( (areaWidth != imageWidth) || (areaHeight != imageHeight) )
                {
                    GdkPixbuf *scaledFrame;
                    scaledFrame = gdk_pixbuf_scale_simple(	frame,
                                                            areaWidth,
                                                            areaHeight,
                                                            GDK_INTERP_BILINEAR); // Best quality
                    //GDK_INTERP_NEAREST); // Best speed

                    pixels = gdk_pixbuf_get_pixels (scaledFrame);
                    rowstride = gdk_pixbuf_get_rowstride(scaledFrame);
                    gdk_draw_rgb_image (widget->window,
                                        widget->style->black_gc,
                                        event->area.x, event->area.y,
                                        event->area.width, event->area.height,
                                        GDK_RGB_DITHER_NORMAL,
                                        pixels,
                                        rowstride);
                    g_object_unref(scaledFrame);
			
                }
            else
                {
                    pixels = gdk_pixbuf_get_pixels (frame);
                    rowstride = gdk_pixbuf_get_rowstride(frame);
                    gdk_draw_rgb_image (widget->window,
                                        widget->style->black_gc,
                                        event->area.x, event->area.y,
                                        event->area.width, event->area.height,
                                        GDK_RGB_DITHER_NORMAL,
                                        pixels,
                                        rowstride);
                }
        }
	return TRUE;
}

static gint menuFileQuit_CB(GtkWidget *widget, gpointer data)
{
	cleanExit();
 
	return TRUE;
}

static gint menuHelpAbout_CB(GtkWidget *widget, gpointer data)
{
#if GTK_CHECK_VERSION(2,6,0)
	const gchar *authors[] = 
        {
            "EmmeBi",
            NULL
        };
	const gchar *license =
		"Released under the terms of the GNU GPL v2.0.\n"
		"The complete license description is contained in the\n"
		"COPYING file included in this distribution.\n"
		"Please refer to this file for complete\n"
		"information about the licensing of YARP.\n"
		"\n"
		"DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE\n"
		"SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS\n"
		"DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS\n"
		"EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE\n"
		"SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT\n"
		"OWNERS AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED\n"
		"INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
		"FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO\n"
		"EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE\n"
		"LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN\n"
		"ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN\n"
		"ONNECTION WITH THE SOFTWARE.\n";

	gtk_show_about_dialog(GTK_WINDOW(mainWindow),
                          "name", "gview",
                          "version", "0.9",
                          "license", license,
                          "website", "http://www.liralab.it",
                          "comments", "Program to display images received on a port.",
                          "authors", authors,
                          NULL);
#else
    printf("Missing functionality on older GTK version, sorry\n");
#endif

	return TRUE;
}


static gint menuImageSize_CB(GtkWidget *widget, gpointer data)
{
	int targetWidth, targetHeight;
    _semaphore.wait();
	targetWidth = _inputImg.width();
	targetHeight = _inputImg.height();
    _semaphore.post();
    if (targetWidth!=0&&targetHeight!=0) {
        gtk_window_resize(GTK_WINDOW(mainWindow), targetWidth, (targetHeight+_occupiedHeight));
    }
	return TRUE;
}

static gint menuImageRatio_CB(GtkWidget *widget, gpointer data)
{
	double ratio;
	int imgWidth, imgHeight;
	int targetWidth, targetHeight;
	int daWidth, daHeight;
    _semaphore.wait();
	imgWidth = _inputImg.width();
	imgHeight = _inputImg.height();
    _semaphore.post();
    if (imgWidth!=0&&imgHeight!=0) {
        daWidth = da->allocation.width;
        daHeight = da->allocation.height;
        ratio = double(imgWidth) / double(imgHeight);
        targetWidth = int(double(daHeight) * ratio);
        targetHeight = daHeight;
        // TO DO : resize DrawingArea Directly
        gtk_window_resize(GTK_WINDOW(mainWindow), targetWidth, (targetHeight+_occupiedHeight));
    }
	return TRUE;
}

static gint menuImageInterval_CB(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	double interval;

	interval = _imgRecv.GetEstimatedInterval();
	dialog = gtk_message_dialog_new (	GTK_WINDOW (mainWindow),
										GTK_DIALOG_MODAL,
										GTK_MESSAGE_INFO,
										GTK_BUTTONS_OK,
										"Estimated interval during last cycle:\n");
#if GTK_CHECK_VERSION(2,6,0)
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%.3f seconds.", interval);
#else
    printf("Missing functionality on older GTK version, sorry\n");
#endif
	gtk_window_set_title (GTK_WINDOW(dialog), "Estimated interval");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	return TRUE;
}

static gint menuFileSingle_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) ) 
        {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSetItem), FALSE);
            gtk_widget_show_all (saveSingleDialog);
		
        } 
	else 
        {
            gtk_widget_hide (saveSingleDialog);	
        }

	return TRUE;
}

static gint menuFileSet_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
#if GTK_CHECK_VERSION(2,6,0)

	if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) ) 
        {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSingleItem), FALSE);
					
            gtk_widget_show_all (saveSetDialog);
        } 
	else 
        {
            gtk_widget_hide (saveSetDialog);
        }

#endif

	return TRUE;
}

static gint menuImageFreeze_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) ) 
        {
            _freezed = true;
    
        } else 
            {
                _freezed = false;
            }

	return TRUE;
}

static gint saveSingleDelete_CB (GtkWidget *widget, gpointer data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSingleItem), FALSE);

	return (TRUE);
}

static gint saveSetDelete_CB (GtkWidget *widget, gpointer data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSetItem), FALSE);

	return (TRUE);
}

static gint saveSingleClicked_CB(GtkWidget *widget, gpointer data)
{
	saveCurrentFrame();
	
	return (TRUE);
}

static gint saveSetStartClicked_CB(GtkWidget *widget, gpointer data)
{
	_savingSet = true;
		
	return (TRUE);
}

static gint saveSetStopClicked_CB(GtkWidget *widget, gpointer data)
{
	_savingSet = false;
		
	return (TRUE);
}

static gint menuImageFramerate_CB(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *stock;

	GtkWidget *label;
	GtkWidget *spinner;
	GtkAdjustment *spinner_adj;
	gint response;

	dialog = gtk_dialog_new_with_buttons ("New Refresh Time",
                                          GTK_WINDOW (mainWindow),
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          GTK_STOCK_CANCEL,
                                          GTK_RESPONSE_CANCEL,
                                          NULL);

	hbox = gtk_hbox_new (FALSE, 8);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
	stock = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start (GTK_BOX (hbox), stock, FALSE, FALSE, 0);
	label = gtk_label_new_with_mnemonic ("Insert new refresh time (in mSec):");
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new (_options.refreshTime, 10.0, 1000.0, 1.0, 5.0, 5.0);
	spinner = gtk_spin_button_new (spinner_adj, 1.0, 0);
	gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 0);
	
	gtk_widget_show_all (hbox);
	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_OK)
        {
            _options.refreshTime = (int) gtk_spin_button_get_value (GTK_SPIN_BUTTON(spinner));
            gtk_timeout_remove (_options.refreshTime);
            timeout_ID = gtk_timeout_add (_options.refreshTime, timeout_CB, NULL);
            updateStatusbar(GTK_STATUSBAR (statusbar));
        }

	gtk_widget_destroy (dialog);
		
	return (TRUE);
}

static gint clickDA_CB (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	int imageX, imageY;
	int clickX, clickY;
	int daWidth, daHeight;
	int imageWidth, imageHeight;
	double ratioX, ratioY;
	
	imageWidth = _imgRecv.GetWidth();
	imageHeight = _imgRecv.GetHeight();
	if ( (imageWidth != 0) && (imageHeight != 0) )
        {
            daWidth = widget->allocation.width;
            daHeight = widget->allocation.height;
            clickX = (int) event->x;
            clickY = (int) event->y;
            ratioX = double(clickX) / double(daWidth);
            ratioY = double(clickY) / double(daHeight);
            imageX = int(imageWidth * ratioX + 0.5);
            imageY = int(imageHeight * ratioY + 0.5);

            printf("Transmitting click information...\n");
            if (_pOutPort!=NULL) {
                yarp::os::Bottle& bot = _pOutPort->prepare();
                bot.clear();
                bot.addInt(imageX);
                bot.addInt(imageY);
                //_pOutPort->Content() = _outBottle;
                _pOutPort->write();
            }
	
        } else {
            printf("I would send a position, but there's no image for scaling\n");
        }

	return TRUE;
}

//-------------------------------------------------
// Non Modal Dialogs
//-------------------------------------------------
GtkWidget* createSaveSingleDialog(void)
{

	GtkWidget *dialog = NULL;
	GtkWidget *button;
	GtkWidget *hbox;
	dialog = gtk_dialog_new ();
	gtk_window_set_title(GTK_WINDOW(dialog), "Save Snapshot");
	gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWindow));
	//gtk_window_resize(GTK_WINDOW(dialog), 185, 40);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	//gtk_window_set_default_size(GTK_WINDOW(dialog), 185, 40);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	gtk_dialog_set_has_separator (GTK_DIALOG(dialog), FALSE);
	hbox = gtk_hbox_new (TRUE, 8); // parameters (gboolean homogeneous_space, gint spacing);
	button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_widget_set_size_request (GTK_WIDGET(button), 150,50);
	gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 16); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (saveSingleClicked_CB), NULL);
	gtk_signal_connect (GTK_OBJECT (dialog), "delete_event", GTK_SIGNAL_FUNC (saveSingleDelete_CB), NULL);
	
	//gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	
	return dialog;
}

GtkWidget* createSaveSetDialog(void)
{
	GtkWidget *dialog = NULL;
	GtkWidget *saveButton;
	GtkWidget *stopButton;
	GtkWidget *hbox;
	dialog = gtk_dialog_new ();
	gtk_window_set_title(GTK_WINDOW(dialog), "Save Image Set");
	gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWindow));
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	//gtk_window_set_default_size(GTK_WINDOW(dialog), 190, 40);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	gtk_dialog_set_has_separator (GTK_DIALOG(dialog), FALSE);
#if GTK_CHECK_VERSION(2,6,0)
	saveButton = gtk_button_new_from_stock(GTK_STOCK_MEDIA_RECORD);
	stopButton = gtk_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
#else
    printf("Missing functionality on older GTK version, sorry\n");
#endif
	gtk_widget_set_size_request (GTK_WIDGET(saveButton), 80,50);
	gtk_widget_set_size_request (GTK_WIDGET(stopButton), 80,50);

	hbox = gtk_hbox_new (TRUE, 8); // parameters (gboolean homogeneous_space, gint spacing);
	gtk_box_pack_start (GTK_BOX (hbox), saveButton, TRUE, TRUE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_box_pack_start (GTK_BOX (hbox), stopButton, TRUE, TRUE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_signal_connect (GTK_OBJECT (saveButton), "clicked", GTK_SIGNAL_FUNC (saveSetStartClicked_CB), NULL);
	gtk_signal_connect (GTK_OBJECT (stopButton), "clicked", GTK_SIGNAL_FUNC (saveSetStopClicked_CB), NULL);
	gtk_signal_connect (GTK_OBJECT (dialog), "delete_event", GTK_SIGNAL_FUNC (saveSetDelete_CB), NULL);

	return dialog;
}

//-------------------------------------------------
// Main Window Menubar
//-------------------------------------------------
GtkWidget* createMenubar(void)
{
    GtkWidget *menubar;

	menubar =  gtk_menu_bar_new ();
	GtkWidget *menuSeparator;	
    // Submenus Items on menubar
    fileItem = gtk_menu_item_new_with_label ("File");
    imageItem = gtk_menu_item_new_with_label ("Image");
    helpItem = gtk_menu_item_new_with_label ("Help");
    // Submenu: File 
    fileMenu = gtk_menu_new();
    fileSingleItem = gtk_check_menu_item_new_with_label ("Save single image..");
    gtk_menu_append( GTK_MENU(fileMenu), fileSingleItem);
    gtk_signal_connect( GTK_OBJECT(fileSingleItem), "toggled", GTK_SIGNAL_FUNC(menuFileSingle_CB), mainWindow);
    fileSetItem = gtk_check_menu_item_new_with_label ("Save a set of images..");
    gtk_menu_append( GTK_MENU(fileMenu), fileSetItem);
    gtk_signal_connect( GTK_OBJECT(fileSetItem), "toggled", GTK_SIGNAL_FUNC(menuFileSet_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(fileMenu), menuSeparator);
    fileQuitItem = gtk_menu_item_new_with_label ("Quit");
    gtk_menu_append( GTK_MENU(fileMenu), fileQuitItem);
    gtk_signal_connect( GTK_OBJECT(fileQuitItem), "activate", GTK_SIGNAL_FUNC(menuFileQuit_CB), mainWindow);
    // Submenu: Image  
    imageMenu = gtk_menu_new();
    imageSizeItem = gtk_menu_item_new_with_label ("Original size");
    gtk_menu_append( GTK_MENU(imageMenu), imageSizeItem);
    gtk_signal_connect( GTK_OBJECT(imageSizeItem), "activate", GTK_SIGNAL_FUNC(menuImageSize_CB), mainWindow);
    imageRatioItem = gtk_menu_item_new_with_label ("Original aspect ratio");
    gtk_menu_append( GTK_MENU(imageMenu), imageRatioItem);
    gtk_signal_connect( GTK_OBJECT(imageRatioItem), "activate", GTK_SIGNAL_FUNC(menuImageRatio_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(imageMenu), menuSeparator);
    imageFreezeItem = gtk_check_menu_item_new_with_label ("Freeze");
    gtk_menu_append( GTK_MENU(imageMenu), imageFreezeItem);
    gtk_signal_connect( GTK_OBJECT(imageFreezeItem), "toggled", GTK_SIGNAL_FUNC(menuImageFreeze_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(imageMenu), menuSeparator);
    imageFramerateItem = gtk_menu_item_new_with_label ("Change refresh interval..");
    gtk_menu_append( GTK_MENU(imageMenu), imageFramerateItem);
    gtk_signal_connect( GTK_OBJECT(imageFramerateItem), "activate", GTK_SIGNAL_FUNC(menuImageFramerate_CB), mainWindow);
    imageIntervalItem = gtk_menu_item_new_with_label ("Show Interval..");
    gtk_menu_append( GTK_MENU(imageMenu), imageIntervalItem);
    gtk_signal_connect( GTK_OBJECT(imageIntervalItem), "activate", GTK_SIGNAL_FUNC(menuImageInterval_CB), mainWindow);
    // Submenu: Help
    helpMenu = gtk_menu_new();	
    helpAboutItem = gtk_menu_item_new_with_label ("About..");
    gtk_menu_append( GTK_MENU(helpMenu), helpAboutItem);
    gtk_signal_connect( GTK_OBJECT(helpAboutItem), "activate", GTK_SIGNAL_FUNC(menuHelpAbout_CB), mainWindow);
    // linking the submenus to items on menubar
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileItem), fileMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(imageItem), imageMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpItem), helpMenu);
    // appending the submenus to the menubar
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), fileItem);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), imageItem);
    gtk_menu_item_set_right_justified (GTK_MENU_ITEM (helpItem), TRUE);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), helpItem);
  
	return menubar;
}

//-------------------------------------------------
// Main Window Statusbar
//-------------------------------------------------
void updateStatusbar (GtkStatusbar  *statusbar)
{
    gchar *msg;
    float fps;
    fps = 1000 / float(_options.refreshTime);
 
    gtk_statusbar_pop (statusbar, 0); // clear any previous message, underflow is allowed 
				    
    msg = g_strdup_printf ("%s - %.1f fps",_options.portName, fps);

    gtk_statusbar_push (statusbar, 0, msg);

    g_free (msg);
}

//-------------------------------------------------
// Main Window 
//-------------------------------------------------
GtkWidget* createMainWindow(void)
{
	GtkRequisition actualSize;
	GtkWidget* window;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "YARP GTK Image Viewer");
	gtk_window_set_default_size(GTK_WINDOW (window), _options.windWidth, _options.windHeight); 
	gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
    // When the window is given the "delete_event" signal (this is given
	// by the window manager, usually by the "close" option, or on the
	// titlebar), we ask it to call the delete_event () function
	// as defined above. The data passed to the callback
	// function is NULL and is ignored in the callback function.
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);
    // Box for main window
	GtkWidget *box;
	box = gtk_vbox_new (FALSE, 0); // parameters (gboolean homogeneous_space, gint spacing);
    gtk_container_add (GTK_CONTAINER (window), box);
	// MenuBar for main window
	menubar = createMenubar();
	gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, TRUE, 0); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
	gtk_widget_size_request(menubar, &actualSize);
	// Drawing Area : here the image will be drawed
	da = gtk_drawing_area_new ();
    g_signal_connect (da, "expose_event", G_CALLBACK (expose_CB), NULL);
	if (_options.outputEnabled == 1)
        {
            g_signal_connect (da, "button_press_event", G_CALLBACK (clickDA_CB), NULL);
            // Ask to receive events the drawing area doesn't normally subscribe to
            gtk_widget_set_events (da, gtk_widget_get_events (da) | GDK_BUTTON_PRESS_MASK);
        }
	gtk_box_pack_start(GTK_BOX(box), da, TRUE, TRUE, 0);
	// StatusBar for main window
	statusbar = gtk_statusbar_new ();
	updateStatusbar(GTK_STATUSBAR (statusbar));
	gtk_box_pack_start (GTK_BOX (box), statusbar, FALSE, TRUE, 0);
	gtk_widget_size_request(statusbar, &actualSize);
	_occupiedHeight += 2*(actualSize.height);

    frame = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, _options.windWidth, _options.windHeight);
	// TimeOut used to refresh the screen
    timeout_ID = gtk_timeout_add (_options.refreshTime, timeout_CB, NULL);

	return window;
}

//-------------------------------------------------
// Service Fuctions
//-------------------------------------------------
void saveCurrentFrame()
{
	char fileName[256];
	sprintf(fileName,"%03d.ppm",_frameN);
	_imgRecv.SaveLastImage(fileName);
}

bool getImage()
{
	bool ret = false;
	ret = _imgRecv.Update();
	if (ret == false)
		return false;

	_semaphore.wait();
	ret = _imgRecv.GetLastImage(&_inputImg);
	_semaphore.post();
		
	return ret;
}



void setOptions(yarp::os::Searchable& options) {
    // switch to subsections if available
    yarp::os::Searchable *network = &options.findGroup("NETWORK");
    yarp::os::Searchable *window = &options.findGroup("WINDOW");
    yarp::os::Searchable *program = &options.findGroup("PROGRAM");

    if (network->isNull()) { network = &options; }
    if (window->isNull()) { window = &options; }
    if (program->isNull()) { program = &options; }

    yarp::os::Value *val;
    if (network->check("PortName",val)||network->check("name",val)) {
        ACE_OS::sprintf(_options.portName, val->asString().c_str());
    }
    if (network->check("NetName",val)||network->check("n",val)) {
        ACE_OS::sprintf(_options.networkName, val->asString().c_str());
    }
    if (network->check("OutPortName",val)||network->check("out",val)) {
        ACE_OS::sprintf(_options.outPortName, val->asString().c_str());
    }
    if (network->check("OutNetName",val)||network->check("neto",val)) {
        ACE_OS::sprintf(_options.outNetworkName, val->asString().c_str());
    }
    if (network->check("RefreshTime",val)||network->check("p",val)) {
        _options.refreshTime = val->asInt();
    }
    if (window->check("PosX",val)||window->check("x",val)) {
        _options.posX = val->asInt();
    }
    if (window->check("PosY",val)||window->check("y",val)) {
        _options.posY = val->asInt();
    }
    if (window->check("Width",val)||window->check("w",val)) {
        _options.windWidth = val->asInt();
    }
    if (window->check("Height",val)||window->check("h",val)) {
        _options.windHeight = val->asInt();
    }
    if (program->check("OutputEnabled",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (program->check("out",val)) {
        _options.outputEnabled = true;
    }
    if (program->check("SaveOptions",val)||program->check("saveoptions",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (program->check("Logpolar",val)||program->check("l",val)) {
        _options.logpolar = val->asInt();
    }
    if (program->check("Fovea",val)||program->check("f",val)) {
        _options.fovea = val->asInt();
    }
}

void parseOptFile(char *fileName)
{
    yarp::os::Property options;
    options.fromConfigFile(fileName);
    setOptions(options);
}

void saveOptFile(char *fileName)
{
    FILE *optFile = NULL;
    optFile = ACE_OS::fopen(_options.fileName,"wt");
    if (optFile == NULL)
        {
            g_print("ERROR: Impossible to save to option file.\n");
            return;
        }
    ACE_OS::fprintf(optFile,"[NETWORK]\n" );
    ACE_OS::fprintf(optFile,"PortName %s\n", _options.portName);
    ACE_OS::fprintf(optFile,"NetName %s\n", _options.networkName);
    ACE_OS::fprintf(optFile,"OutPortName %s\n", _options.outPortName);
    ACE_OS::fprintf(optFile,"OutNetName %s\n", _options.outNetworkName);
    ACE_OS::fprintf(optFile,"[WINDOW]\n" );
    ACE_OS::fprintf(optFile,"RefreshTime %d\n", _options.refreshTime);
    ACE_OS::fprintf(optFile,"PosX %d\n", _options.posX);
    ACE_OS::fprintf(optFile,"PosY %d\n", _options.posY);
    ACE_OS::fprintf(optFile,"Width %d\n", _options.windWidth);
    ACE_OS::fprintf(optFile,"Height %d\n", _options.windHeight);
    ACE_OS::fprintf(optFile,"[PROGRAM]\n" );
    ACE_OS::fprintf(optFile,"OutputEnables %d\n", _options.outputEnabled);
    ACE_OS::fprintf(optFile,"SaveOptions %d\n", _options.saveOnExit);
    ACE_OS::fprintf(optFile,"Logpolar %d\n", _options.logpolar);
    ACE_OS::fprintf(optFile,"Fovea %d\n", _options.fovea);
	
    ACE_OS::fclose(optFile);
}

void parseParameters(int argc, char* argv[])
{
    yarp::os::Property options;
    if (argc==2) {
        // user did not use flags, just gave a port name
        // might as well allow this
        options.put("name",argv[1]);
    } else {
        options.fromCommand(argc,argv);
    }
    setOptions(options);
}

void setOptionsToDefault()
{
	// Options defaults
	_options.refreshTime = 100;
	ACE_OS::sprintf(_options.portName,"/yarpview/i:img");
	ACE_OS::sprintf(_options.networkName, "default");
	ACE_OS::sprintf(_options.outPortName,"/yarpview/o:point");
	ACE_OS::sprintf(_options.outNetworkName, "default");
	_options.outputEnabled = 0;
	_options.windWidth = 300;
	_options.windHeight = 300;
	_options.posX = 100;
	_options.posY = 100;
	ACE_OS::sprintf(_options.fileName,"yarpview.conf");
	_options.saveOnExit = 0;
	_options.logpolar = 0;
	_options.fovea = 0;

}

bool openPorts()
{
	bool ret = false;
	//int res = 0;
	// Registering Port(s)
    //reduce verbosity --paulfitz
	//g_print("Registering port %s on network %s...\n", _options.portName, _options.networkName);
	ret = _imgRecv.Connect(_options.portName, _options.networkName);
	if (ret == true)
        {
            //reduce verbosity --paulfitz
            //g_print("Port registration succeed!\n");
        }
	else
        {
            g_print("ERROR: Port registration failed.\nQuitting, sorry.\n");
            return false;
        }
	if (_options.outputEnabled == 1)
        {
		
            _pOutPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
            g_print("Registering port %s on network %s...\n", _options.outPortName, _options.outNetworkName);
            bool ok = _pOutPort->open(_options.outPortName);
            if  (ok)
                g_print("Port registration succeed!\n");
            else 
                {
                    g_print("ERROR: Port registration failed.\nQuitting, sorry.\n");
                    return false;
                }

        }

	return true;
}

void closePorts()
{
	bool ret = false;
	//int res = 0;
    //reduce verbosity --paulfitz
	//g_print("Unregistering port(s)...\n");
	ret = _imgRecv.Disconnect();
	if (ret)
        {
            //reduce verbosity --paulfitz
            //g_print("Port %s unregistration succeed!\n", _options.portName);
        }
	else
        {
            g_print("ERROR: Port %s unregistration failed.\n", _options.portName);
        }
	
	if (_options.outputEnabled == 1)
        {
            _pOutPort->close();
            bool ok = true;
            if  (ok)
                g_print("Port %s unregistration succeed!\n", _options.outPortName);
            else 
                g_print("ERROR: Port %s unregistration failed.\n", _options.outPortName);
            delete _pOutPort;
            _pOutPort = NULL;
        }
}

void setUp()
{
	if (_options.logpolar == 0)
		_imgRecv.SetLogopolar(false);
	else
		_imgRecv.SetLogopolar(true);
	
	if (_options.fovea == 0)
		_imgRecv.SetFovea(false);
	else
		_imgRecv.SetFovea(true);
	
	if (openPorts() == false)
		ACE_OS::exit(1);
	
	_inputImg.resize(_options.windWidth, _options.windHeight);
}

void cleanExit()
{
	g_source_remove (timeout_ID);
	timeout_ID = 0;
	closePorts();
	if (_options.saveOnExit != 0)
		saveOptFile(_options.fileName);
	if (frame)
		g_object_unref(frame);
	// Exit from application
	gtk_main_quit ();
    deleteObjects();
}

//-------------------------------------------------
// Main
//-------------------------------------------------
#undef main //ace leaves a "main" macro defined

int myMain(int argc, char* argv[])
{
    yarp::os::Network::init();
    createObjects();
    //reduce verbosity --paulfitz
    //fprintf(stderr, "Starting as console app\n");
	// Global variables init
	_frameN = 0;
	_savingSet = false;
	_freezed = false;
	_occupiedHeight = 0;
	timeout_ID = 0;
	setOptionsToDefault();
	// Parse option file parameters
	parseOptFile(_options.fileName);
	// Parse command line parameters
	parseParameters(argc, argv);
	// Setting Up Program
	setUp();

    //reduce verbosity --paulfitz
	//g_print("Starting application..\n");
	
	// This is called in all GTK applications. Arguments are parsed
	// from the command line and are returned to the application.
    gtk_init (&argc, &argv);

	// create a new window
    mainWindow = createMainWindow();
	
	// Non Modal Dialogs
#if GTK_CHECK_VERSION(2,6,0)
	saveSingleDialog = createSaveSingleDialog();
	saveSetDialog = createSaveSetDialog();
#else
    printf("Functionality omitted for older GTK version\n");
#endif
	// Shows all widgets in main Window
    gtk_widget_show_all (mainWindow);
	gtk_window_move(GTK_WINDOW(mainWindow), _options.posX, _options.posY);
	// All GTK applications must have a gtk_main(). Control ends here
	// and waits for an event to occur (like a key press or
	// mouse event).

	gtk_main ();

    yarp::os::Network::fini();

    return 0;
}

#ifdef WIN32
#include <windows.h>
// win32 non-console applications define WinMain as the
// entry point for the linker
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    return myMain (__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
    return myMain(argc, argv);
}
#endif