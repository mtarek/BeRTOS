#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

from PyQt4.QtGui import *

from BWizardPage import *
import bertos_utils

from const import *

class BCreationPage(BWizardPage):
    
    def __init__(self):
        BWizardPage.__init__(self, UI_LOCATION + "/project_creation.ui")
        self.setTitle(self.tr("Create the BeRTOS project"))
        self._setupUi()
        self._connectSignals()
    
    def _setupUi(self):
        self._confirmGroup = QWidgetGroup(self.pageContent.summaryTree,
                                            self.pageContent.createButton)
        self._workingGroup = QWidgetGroup(self.pageContent.spinnerLabel)
        self._workingGroup.setVisible(False)
        self._finalGroup = QWidgetGroup(self.pageContent.iconLabel,
                                            self.pageContent.textLabel)
        self._finalGroup.setVisible(False)
    
    def _connectSignals(self):
        self.connect(self.pageContent.createButton, SIGNAL("clicked(bool)"), self._createProject)
    
    def _createProject(self):
        self._confirmGroup.setVisible(False)
        #self._workingGroup.setVisible(True)
        #self._movie = QMovie("images/load_spinner.gif")
        #print self._movie.isValid()
        #self.pageContent.spinnerLabel.setMovie(self._movie)
        #self._movie.start()
        bertos_utils.createBertosProject(self.wizard().project())
        #del self._movie
        #self._workingGroup.setVisible(False)
        self._finalGroup.setVisible(True)
        self.emit(SIGNAL("completeChanged()"))
    
    def isComplete(self):
        return self._finalGroup.isVisible()

class QWidgetGroup(QObject):
    """
    Container class, this class contains widgets and permit to set some
    properties of the contained widgets at the same time.
    """
    def __init__(self, *elements):
        self._widgets = []
        for element in elements:
            self._widgets.append(element)
    
    def addWidget(self, widget):
        if widget not in self._widgets:
            self._widgets.append(widget)
    
    def setVisible(self, visible):
        for widget in self._widgets:
            widget.setVisible(visible)
    
    def isVisible(self):
        for widget in self._widgets:
            if not widget.isVisible():
                return False
        return True
