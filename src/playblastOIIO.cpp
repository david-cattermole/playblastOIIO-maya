/*
 * Implement a playblast command that uses OpenImageIO to write data.
 * Replaces the need for 'maya.cmds.playblast' command.
 *
 * Based on 'blast2Cmd.cpp' from the Maya Devkit.
 *
 */

#include <playblastOIIO.h>


playblastOIIOCmd::playblastOIIOCmd()
        : // m_preRenderNotificationName("playblastOIIOCmdPreRender"),
          // m_preRenderNotificationSemantic(MHWRender::MPassContext::kBeginRenderSemantic),
          m_postRenderNotificationName("playblastOIIOCmdPostRender"),
          m_postRenderNotificationSemantic(MHWRender::MPassContext::kEndRenderSemantic),
          // m_preSceneRenderNotificationName("playblastOIIOCmdPreSceneRender"),
          // m_preSceneRenderNotificationSemantic(MHWRender::MPassContext::kBeginSceneRenderSemantic),
          // m_postSceneRenderNotificationName("playblastOIIOCmdPostSceneRender"),
          // m_postSceneRenderNotificationSemantic(MHWRender::MPassContext::kEndSceneRenderSemantic),
          m_width(0),
          m_height(0)
{
}

playblastOIIOCmd::~playblastOIIOCmd() {
}

void *playblastOIIOCmd::creator() {
    return (void *) (new playblastOIIOCmd);
}

// Add flags to the command syntax
MSyntax playblastOIIOCmd::newSyntax() {
    MSyntax syntax;
    syntax.addFlag(kFilenameFlag, kFilenameFlagLong, MSyntax::kString);
    syntax.addFlag(kStartFrameFlag, kStartFrameFlagLong, MSyntax::kTime);
    syntax.addFlag(kEndFrameFlag, kEndFrameFlagLong, MSyntax::kTime);
    syntax.addFlag(kImageSizeFlag, kImageSizeFlagLong, MSyntax::kUnsigned, MSyntax::kUnsigned);
    syntax.addFlag(kUseOIIOFlag, kUseOIIOFlagLong, MSyntax::kBoolean);
    return syntax;
}

/*
 * Parse command line arguments:
 * 1) Filename (required)
 * 2) Start time. Defaults to 0
 * 3) End time. Defaults to 1
*/
MStatus playblastOIIOCmd::parseArgs(const MArgList &args) {
    MStatus status = MStatus::kSuccess;
    MArgDatabase argData(syntax(), args);

    m_start = 0.0;
    m_end = 1.0;
    m_width = 0;
    m_height = 0;
    m_useOIIO = 0;

    if (argData.isFlagSet(kFilenameFlag)) {
        status = argData.getFlagArgument(kFilenameFlag, 0, m_filename);
    } else {
        return MStatus::kFailure;
    }
    if (argData.isFlagSet(kStartFrameFlag)) {
        argData.getFlagArgument(kStartFrameFlag, 0, m_start);
    }
    if (argData.isFlagSet(kEndFrameFlag)) {
        argData.getFlagArgument(kEndFrameFlag, 0, m_end);
    }
    if (argData.isFlagSet(kImageSizeFlag)) {
        argData.getFlagArgument(kImageSizeFlag, 0, m_width);
        argData.getFlagArgument(kImageSizeFlag, 1, m_height);
    }
    if (argData.isFlagSet(kUseOIIOFlag)) {
        argData.getFlagArgument(kUseOIIOFlag, 0, m_useOIIO);
    }
    return status;
}

//// Print out the pass identifier and pass semantics
//void playblastOIIOCmd::printPassInformation(MHWRender::MDrawContext &context) {
//    const MHWRender::MPassContext &passCtx = context.getPassContext();
//    const MString &passId = passCtx.passIdentifier();
//    const MStringArray &passSem = passCtx.passSemantics();
//
//    printf("\tPass Identifier = %s\n", passId.asChar());
//    for (unsigned int i = 0; i < passSem.length(); i++) {
//        printf("\tPass semantic: %s\n", passSem[i].asChar());
//    }
//}

//void playblastOIIOCmd::preSceneCallback(MHWRender::MDrawContext &context, void *clientData) {
//    // Get back command
//    playblastOIIOCmd *cmd = (playblastOIIOCmd *) clientData;
//    if (!cmd)
//        return;
//    cmd->printPassInformation(context);
//}

//void playblastOIIOCmd::postSceneCallback(MHWRender::MDrawContext &context, void *clientData) {
//    // Get back command
//    playblastOIIOCmd *cmd = (playblastOIIOCmd *) clientData;
//    if (!cmd)
//        return;
//     cmd->printPassInformation(context);
//}

//void playblastOIIOCmd::preFrameCallback(MHWRender::MDrawContext &context, void *clientData) {
//    // Get back command
//    playblastOIIOCmd *cmd = (playblastOIIOCmd *) clientData;
//    if (!cmd)
//        return;
//    cmd->printPassInformation(context);
//}

// Callback which is called at end of render to perform the capture.
// Client data contains a reference back to the command to allow for the
// capture options to be read.
void playblastOIIOCmd::captureCallback(MHWRender::MDrawContext &context, void *clientData) {
    // Get back command
    playblastOIIOCmd *cmd = (playblastOIIOCmd *) clientData;
    if (!cmd)
        return;

    // cmd->printPassInformation(context);

    MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
    if (!renderer)
        return;

    // Create a final frame name of:
    // <filename>.<framenumber>.<frameExtension>
    MString frameName(cmd->m_filename);
    frameName += ".";
    frameName += cmd->m_currentTime.value();

    bool saved = false;

    /*
     * The following is one example of how to retrieve pixels and store them to disk.
     *
     * The most flexible way is to get access to the raw data using MRenderTarget::rawData(), perform
     * any custom saving as desired, and then use MRenderTarget::freeRawData().
     *
     * Note that context.getCurrentDepthRenderTarget() can be used to access the depth buffer.
     */
    OIIO::TypeDesc type_desc = OIIO::TypeDesc::UNKNOWN;
    const MHWRender::MRenderTarget *colorTarget = context.getCurrentColorRenderTarget();
    if (colorTarget) {
        // Query for the target format. If it is floating point then we switch
        // it EXR so that we save properly.
        MString frameExtension;
        MHWRender::MRenderTargetDescription desc;
        colorTarget->targetDescription(desc);
        int width = desc.width();
        int height = desc.height();
        int channels = 0;
        MHWRender::MRasterFormat format = desc.rasterFormat();
        switch (format) {
            case MHWRender::kR16G16B16A16_FLOAT:
                type_desc = OIIO::TypeDesc::HALF;
                frameExtension = ".exr";
                channels = 4;
                break;
            case MHWRender::kR32G32B32_FLOAT:
                type_desc = OIIO::TypeDesc::FLOAT;
                frameExtension = ".exr";
                channels = 3;
                break;
            case MHWRender::kR32G32B32A32_FLOAT:
                type_desc = OIIO::TypeDesc::FLOAT;
                frameExtension = ".exr";
                channels = 4;
                break;
            case MHWRender::kR8G8B8A8_UNORM:
            case MHWRender::kB8G8R8A8:
            case MHWRender::kA8B8G8R8:
                type_desc = OIIO::TypeDesc::UINT8;
                frameExtension = ".iff";
                channels = 4;
                break;
            default:
                frameExtension = "";
                break;
        };
        if (frameExtension.length() == 0)
            return;
        frameName += frameExtension;


        if (!cmd->m_useOIIO) {
            // Native Maya image writing.

            // Get a copy of the render target. We get it back as a texture to
            // allow to use the "save texture" method on the texture manager for
            // this example.
            MHWRender::MTextureManager *textureManager = renderer->getTextureManager();
            MHWRender::MTexture *colorTexture = context.copyCurrentColorRenderTargetToTexture();
            if (colorTexture) {
                // Save the texture to disk
                MStatus status = textureManager->saveTexture(colorTexture, frameName);
                saved = (status == MStatus::kSuccess);

                // When finished with the texture, release it.
                textureManager->releaseTexture(colorTexture);
            }
        }
        else
        {
            // OpenImageIO image writing.
            std::string filename = frameName.asChar();
            OIIO::ImageOutput *img_out = OIIO::ImageOutput::create(filename);
            if (!img_out) {
                std::cerr << "OpenImageIO Error: Writing data failed!" << std::endl;
                return;
            }

            int rowPitch = 0;
            int slicePitch = 0;
            void *rawData = const_cast<MHWRender::MRenderTarget *>(colorTarget)->rawData(rowPitch, slicePitch);
            OIIO::ImageSpec img_out_spec(width, height, channels, type_desc);
            // TODO: Pick the optimal compression method.
            // Do we want to maximise read-speed, write speed, or both?
            // img_out_spec.attribute("compression", "zip"); // Overall good speed vs compression - default.
            // img_out_spec.attribute("compression", "zips"); // Seems slightly faster/slower than 'zip'.
            // img_out_spec.attribute("compression", "piz"); // Good for photos, larger than zip, slower to compress.
            // img_out_spec.attribute("compression", "b44a"); // Good for CG images with large flat colours areas.
            // img_out_spec.attribute("compression", "b44"); // Fast reading and decoding.
            img_out->open(filename, img_out_spec);
            saved = img_out->write_image(type_desc, rawData,
                                         OIIO::AutoStride,
                                         OIIO::AutoStride,
                                         OIIO::AutoStride);
            img_out->close();
            delete img_out;
            colorTarget->freeRawData(rawData);
        }

        // Release reference to the color target
        const MHWRender::MRenderTargetManager *targetManager = renderer->getRenderTargetManager();
        targetManager->releaseRenderTarget((MHWRender::MRenderTarget *) colorTarget);
    }

    // Output some status information about the save
    char msgBuffer[256];
    if (!saved) {
        sprintf(msgBuffer,
                "Failed to color render target to %s.",
                frameName.asChar());
        MGlobal::displayError(msgBuffer);
    } else {
        sprintf(msgBuffer,
                "Captured color render target to %s.",
                frameName.asChar());
        MGlobal::displayInfo(msgBuffer);
    }
}


// Perform the blast command on the current 3d view
//
// 1) Setting up a post render callback on VP2.
// 2) iterating from start to end time.
// 3) During the callback writing the current VP2 render target to disk
MStatus playblastOIIOCmd::doIt(const MArgList &args) {
    MStatus status = MStatus::kFailure;

    MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
    if (!renderer) {
        MGlobal::displayError("VP2 renderer not initialized.");
        return status;
    }

    status = parseArgs(args);
    if (!status) {
        char msgBuffer[256];
        sprintf(msgBuffer, "Failed to parse args for %s command.\n", commandName);
        MGlobal::displayError(msgBuffer);
        return status;
    }

    // Find then current 3dView.
    M3dView view = M3dView::active3dView(&status);
    if (!status) {
        MGlobal::displayError("Failed to find an active 3d view to capture.");
        return status;
    }

    // Set up notification of end of render. Send the blast command
    // to allow accessing data members.
    renderer->addNotification(captureCallback,
                              m_postRenderNotificationName,
                              m_postRenderNotificationSemantic,
                              (void *) this);
    // Sample code to show additional notification usage
    // renderer->addNotification(preFrameCallback,
    //                           m_preRenderNotificationName,
    //                           m_preRenderNotificationSemantic,
    //                           (void *) this);
    // renderer->addNotification(preSceneCallback,
    //                           m_preSceneRenderNotificationName,
    //                           m_preSceneRenderNotificationSemantic,
    //                           (void *) this);
    // renderer->addNotification(postSceneCallback,
    //                           m_postSceneRenderNotificationName,
    //                           m_postSceneRenderNotificationSemantic,
    //                           (void *) this);

    // Check for override image size.
    bool setOverride = (m_width > 0 && m_height > 0);
    if (setOverride) {
        renderer->setOutputTargetOverrideSize(m_width, m_height);
    }
    // Temporarily turn off on-screen updates
    renderer->setPresentOnScreen(false);

    for (m_currentTime = m_start; m_currentTime <= m_end; m_currentTime++) {
        MAnimControl::setCurrentTime(m_currentTime);
        bool all = false;
        bool force = true;
        // bool offscreen = true;
        view.refresh(all, force);
    }

    // Remove notification of end of render
    renderer->removeNotification(m_postRenderNotificationName,
                                 m_postRenderNotificationSemantic);
    // renderer->removeNotification(m_preRenderNotificationName,
    //                              m_preRenderNotificationSemantic);
    // renderer->removeNotification(m_preSceneRenderNotificationName,
    //                              m_preSceneRenderNotificationSemantic);
    // renderer->removeNotification(m_postSceneRenderNotificationName,
    //                              m_postSceneRenderNotificationSemantic);

    // Restore off on-screen updates
    renderer->setPresentOnScreen(true);

    // Disable target size override
    renderer->unsetOutputTargetOverrideSize();

    return status;
}



