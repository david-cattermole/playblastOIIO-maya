/*
 * Implement a playblast command that uses OpenImageIO to write data.
 * Replaces the need for 'maya.cmds.playblast' command.
 *
 * Based on 'blast2Cmd.cpp' from the Maya Devkit.
 */

#ifndef PLAYBLAST_OIIO_H
#define PLAYBLAST_OIIO_H

// Standard
#include <cstdio>

// Maya
#include <maya/MGlobal.h>
#include <maya/MImage.h>
#include <maya/MIOStream.h>

#include <maya/MObject.h>
#include <maya/MString.h>

#include <maya/MArgList.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

#include <maya/M3dView.h>
#include <maya/MAnimControl.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MDrawContext.h>
#include <maya/MRenderTargetManager.h>
#include <maya/MRenderUtilities.h>

// OpenImageIO
#include <OpenImageIO/ustring.h>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/version.h>

#if (OIIO_VERSION < 10100)
namespace OIIO = OIIO_NAMESPACE;
#endif

// Command arguments and command name
#define kFilenameFlag       "-f"
#define kFilenameFlagLong   "-filename"

#define kStartFrameFlag     "-sf"
#define kStartFrameFlagLong "-startFrame"

#define kEndFrameFlag       "-ef"
#define kEndFrameFlagLong   "-endFrame"

#define kImageSizeFlag      "-is"
#define kImageSizeFlagLong  "-imageSize"

#define commandName         "playblastOIIO"

// Command class declaration
class playblastOIIOCmd : public MPxCommand {
public:
    playblastOIIOCmd();

    virtual            ~playblastOIIOCmd();

    MStatus doIt(const MArgList &args);

    static MSyntax newSyntax();

    static void *creator();

private:
    MStatus parseArgs(const MArgList &args);

    // Capture options
    MString m_filename;    // File name
    MTime m_start;         // Start time
    MTime m_end;           // End time

    // Temporary to keep track of current time being captured
    MTime m_currentTime;

    // Override width and height
    unsigned int m_width;
    unsigned int m_height;

    // VP2 capture notification information
    MString m_postRenderNotificationName;
    MString m_postRenderNotificationSemantic;

    static void captureCallback(MHWRender::MDrawContext &context, void *clientData);

    // This code is not required for the logic of this command put put in for
    // completeness to show the additional possible callbacks. Will only be set up
    // if the debug flag m_debugTraceNotifications is set to true. It is set to false by default.
    /* The debug output could look something like this for a 2 pass render using the printPassInformation() utility:

        Pass Identifier = blast2CmdPreRender
        Pass semantic: colorPass
        Pass semantic: beginRender

        Pass Identifier = blast2CmdPreSceneRender
        Pass semantic: colorPass
        Pass semantic: beginSceneRender
        Pass Identifier = blast2CmdPostSceneRender
        Pass semantic: colorPass
        Pass semantic: endSceneRender

        Pass Identifier = blast2CmdPreSceneRender
        Pass semantic: colorPass
        Pass semantic: beginSceneRender
        Pass Identifier = blast2CmdPostSceneRender
        Pass semantic: colorPass
        Pass semantic: endSceneRender

        Pass Identifier = blas2CmdPostRender
        Pass semantic: endRender
    */
//    bool m_debugTraceNotifications;
//    MString m_preRenderNotificationName;
//    MString m_preRenderNotificationSemantic;
//
//    static void preFrameCallback(MHWRender::MDrawContext &context, void *clientData);

//    MString m_preSceneRenderNotificationName;
//    MString m_preSceneRenderNotificationSemantic;
//
//    static void preSceneCallback(MHWRender::MDrawContext &context, void *clientData);
//
//    MString m_postSceneRenderNotificationName;
//    MString m_postSceneRenderNotificationSemantic;
//
//    static void postSceneCallback(MHWRender::MDrawContext &context, void *clientData);

    // Utility to print out pass information at notification time
    void printPassInformation(MHWRender::MDrawContext &context);
};

#endif // PLAYBLAST_OIIO_H