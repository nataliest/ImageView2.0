/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ImageViewAudioProcessorEditor::ImageViewAudioProcessorEditor (ImageViewAudioProcessor& p)
: AudioProcessorEditor (p),
//processor (p), // not required if we have the getProcesson() methon implemented
//d(new DragAndDrop(*(p.filepath))), // could be initialized in the initialization list

fileFilter ("*.jpeg;*.jpg;*.png;*.gif", "*", "Image Filter"),
fileDirectoryThread ("Image File Scanner"),
dirContentsList (&fileFilter, fileDirectoryThread),
fileTree (dirContentsList),
resizerTop (&layout, 1, false),
resizerMid(&layout, 3, false),
resizerBottom(&layout, 5, false)

{
    pathOrFile = *(p.filepath);
    dragDropComponent = new DragAndDrop(*(p.filepath));
    addAndMakeVisible(dragDropComponent);
    setOpaque (true);
    dirContentsList.setDirectory (pathOrFile, true, true);
    fileDirectoryThread.startThread (1);
    
    fileTree.addListener (this);
    
    fileTree.setColour (TreeView::backgroundColourId, Colours::whitesmoke.withAlpha (0.6f));
    
    addAndMakeVisible (resizerTop);
    
    addAndMakeVisible (fileTree);
    
    addAndMakeVisible (resizerMid);
    
    addAndMakeVisible (imagePreview);
    
    addAndMakeVisible (resizerBottom);
    
    if (*(p.comments) != "") {
        textbox.setText(*(p.comments));
    }
    
    textbox.setColour(TextEditor::backgroundColourId, Colours::lightgrey);
    textbox.setColour(TextEditor::textColourId, Colours::darkgrey);
    textbox.setMultiLine(true);
    textbox.setReturnKeyStartsNewLine(true);
    textbox.setScrollbarsShown(true);
    textbox.setTextToShowWhenEmpty("Add your comments here...", Colours::grey);
    addAndMakeVisible (textbox);
    
    // d&d
    layout.setItemLayout (0, -0.05, -0.15, -0.1);
    
    // resizerTop
    layout.setItemLayout (1, 3, 3, 3);
    
    // fileTree
    layout.setItemLayout (2, -0.1, -0.9, -0.3);
    
    // resizerMid
    layout.setItemLayout (3, 3, 3, 3);
    
    // imagePreview
    layout.setItemLayout (4, -0.1, -0.9, -0.55);
    
    // resizerBottom
    layout.setItemLayout (5, 3, 3, 3);
    
    // textbox
    layout.setItemLayout (6, -0.1, -0.9, -0.05);
    
    setResizable(true, true);
    
    
    setSize (p.lastUIWidth, p.lastUIHeight);
    setResizeLimits (400, 400, 1200, 600);
}

ImageViewAudioProcessorEditor::~ImageViewAudioProcessorEditor()
{
    if (textbox.getText() != *(getProcessor().comments)) {
        *(getProcessor().comments) = textbox.getText();
    }
    fileTree.removeListener (this);
    
    delete dragDropComponent;
}

//==============================================================================
void ImageViewAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);   // clear the background
    g.setColour (Colours::black);
    g.drawRect (getLocalBounds(), 0.5);
    browserRootChanged(dragDropComponent->getPath());
    *(getProcessor().filepath) = dragDropComponent->getPath();
    if (dragDropComponent->getFiletreeChanged()) {
        dirContentsList.refresh();
        dragDropComponent->setFiletreeChanged(false);
    }
}

void ImageViewAudioProcessorEditor::resized()
{
    // subcomponents in your editor..
    Rectangle<int> r (getLocalBounds());
    //
    // make a list of two of our child components that we want to reposition
    Component* comps[] = { dragDropComponent, &resizerTop, &fileTree, &resizerMid, &imagePreview, &resizerBottom, &textbox };
    
    // this will position the 3 components, one above the other, to fit
    // vertically into the rectangle provided.
    layout.layOutComponents (comps, 7, r.getX(), r.getY(), r.getWidth(), r.getHeight(), true, true);
    
    getProcessor().lastUIWidth = getWidth();
    getProcessor().lastUIHeight = getHeight();
    
}

void ImageViewAudioProcessorEditor::selectionChanged() {
    
    const File selectedFile (fileTree.getSelectedFile());
    
    if (selectedFile.existsAsFile())
        imagePreview.setImage (ImageCache::getFromFile (selectedFile));
}

void ImageViewAudioProcessorEditor::browserRootChanged (const File& f)  {
    dirContentsList.setDirectory (f, true, true);
}

void ImageViewAudioProcessorEditor::filesDropped (const StringArray& files, int /*x*/, int /*y*/)
{
    File file;
    if (files.size() == 1) {
        file = files[0];
        if (file.isDirectory()) { // update path only if 1 directory is dropped
            dragDropComponent->setPath(files.joinIntoString("\n"));
        } else {
            dragDropComponent->copyFilesToCurrentDirectory(files);
        }
    } else if (files.size() > 0) {
        dragDropComponent->copyFilesToCurrentDirectory(files);
    }
    dragDropComponent->setMess("Current directory: " + dragDropComponent->getPath());
    
    smthIsBeingDraggedOver = false;
    repaint();
}
