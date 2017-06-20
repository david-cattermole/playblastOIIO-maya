"""

"""

import maya.cmds


# Get the model editor with focus.
def getActiveModelEditor():
    panel = maya.cmds.getPanel(withFocus=True)
    panelType = maya.cmds.getPanel(typeOf=panel)
    if panelType != 'modelPanel':
        return ''
    modelEd = maya.cmds.modelPanel(panel, modelEditor=True, query=True)
    return modelEd


# Get all model editors
def getAllModelEditors():
    panels = maya.cmds.getPanel(type='modelPanel')
    result = []
    for panel in panels:
        editor = maya.cmds.modelPanel(panel, q=True, modelEditor=True)
        result.append(editor)
    return result


def disableColorManagement(modelEditors=None):
    """
    Disables color management for modelEditors (3D viewports).

    :param modelEditors: List of Model Editors to disable.
    :type modelEditors: list of basestring
    :return: bool, True
    """
    if modelEditors is None:
        modelEditors = getAllModelEditors()
    for editor in modelEditors:
        setModelEditorColorManagement(editor, False)
    return True


def enableColorManagement(modelEditors=None):
    """
    Disables color management for modelEditors (3D viewports).

    :param modelEditors: List of Model Editors to disable.
    :type modelEditors: list of basestring
    :return: bool, True
    """
    if modelEditors is None:
        modelEditors = getAllModelEditors()
    for editor in modelEditors:
        setModelEditorColorManagement(editor, True)
    return True


def setModelEditorColorManagement(editor, value):
    maya.cmds.modelEditor(editor, e=True, cmEnabled=value)


def main():
    """Main run function. Will use playblastOIIO to generate viewport frames."""
    maya.cmds.loadPlugin("playblastOIIO")
    maya.cmds.playblastOIIO(filepath='/path/to/output_filename', startFrame=1, endFrame=24, imageSize=(1920, 1080))
    pass