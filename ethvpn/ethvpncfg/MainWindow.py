# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MainWindow.ui'
#
# Created: Sat Jul 23 23:34:03 2011
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(442, 317)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.gridLayout = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.cfgButton = QtGui.QPushButton(self.centralwidget)
        self.cfgButton.setObjectName(_fromUtf8("cfgButton"))
        self.gridLayout.addWidget(self.cfgButton, 1, 0, 1, 1)
        spacerItem = QtGui.QSpacerItem(247, 24, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem, 1, 1, 1, 1)
        self.rfrshButton = QtGui.QPushButton(self.centralwidget)
        self.rfrshButton.setObjectName(_fromUtf8("rfrshButton"))
        self.gridLayout.addWidget(self.rfrshButton, 1, 2, 1, 1)
        self.deviceList = QtGui.QTreeView(self.centralwidget)
        self.deviceList.setObjectName(_fromUtf8("deviceList"))
        self.gridLayout.addWidget(self.deviceList, 0, 0, 1, 3)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 442, 22))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menuDevice = QtGui.QMenu(self.menubar)
        self.menuDevice.setObjectName(_fromUtf8("menuDevice"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.actionConfigure = QtGui.QAction(MainWindow)
        self.actionConfigure.setObjectName(_fromUtf8("actionConfigure"))
        self.actionRefresh = QtGui.QAction(MainWindow)
        self.actionRefresh.setObjectName(_fromUtf8("actionRefresh"))
        self.actionQuit = QtGui.QAction(MainWindow)
        self.actionQuit.setObjectName(_fromUtf8("actionQuit"))
        self.menuDevice.addAction(self.actionConfigure)
        self.menuDevice.addAction(self.actionRefresh)
        self.menuDevice.addAction(self.actionQuit)
        self.menubar.addAction(self.menuDevice.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QObject.connect(self.actionQuit, QtCore.SIGNAL(_fromUtf8("activated()")), MainWindow.close)
        QtCore.QObject.connect(self.rfrshButton, QtCore.SIGNAL(_fromUtf8("clicked()")), MainWindow.refresh)
        QtCore.QObject.connect(self.actionRefresh, QtCore.SIGNAL(_fromUtf8("activated()")), MainWindow.refresh)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "VPN devices", None, QtGui.QApplication.UnicodeUTF8))
        self.cfgButton.setText(QtGui.QApplication.translate("MainWindow", "Configure...", None, QtGui.QApplication.UnicodeUTF8))
        self.rfrshButton.setText(QtGui.QApplication.translate("MainWindow", "&Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.menuDevice.setTitle(QtGui.QApplication.translate("MainWindow", "Device", None, QtGui.QApplication.UnicodeUTF8))
        self.actionConfigure.setText(QtGui.QApplication.translate("MainWindow", "Configure...", None, QtGui.QApplication.UnicodeUTF8))
        self.actionRefresh.setText(QtGui.QApplication.translate("MainWindow", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.actionRefresh.setShortcut(QtGui.QApplication.translate("MainWindow", "F5", None, QtGui.QApplication.UnicodeUTF8))
        self.actionQuit.setText(QtGui.QApplication.translate("MainWindow", "Quit", None, QtGui.QApplication.UnicodeUTF8))

