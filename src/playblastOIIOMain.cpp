/*
 * Implement a playblast command that uses OpenImageIO to write data.
 * Replaces the need for 'maya.cmds.playblast' command.
 *
 * Based on 'blast2Cmd.cpp' from the Maya Devkit.
 */

#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>

#include <playblastOIIO.h>

// Plugin load function
MStatus initializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin(obj, PLUGIN_COMPANY, "1.0", "Any");

    status = plugin.registerCommand(commandName,
                                    playblastOIIOCmd::creator,
                                    playblastOIIOCmd::newSyntax);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

    return status;
}

// Plugin unload function
MStatus uninitializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterCommand(commandName);
    if (!status) {
        status.perror("deregisterCommand");
        return status;
    }

    return status;
}