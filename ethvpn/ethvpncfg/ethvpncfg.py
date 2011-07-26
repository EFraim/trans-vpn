#!/usr/bin/python
from MainWindow import Ui_MainWindow
from PyQt4.QtCore import QAbstractListModel, QModelIndex, pyqtSignal, Qt, QVariant
from PyQt4.QtGui import QApplication, QDialog, QMainWindow, QStandardItemModel, QStandardItem
import pyudev
from pyudev.pyqt4 import QUDevMonitorObserver
import sys
import configure

DEVICE_ID = '16c0'
NET_ID = '03ea'
CON_ID = '03eb'

class DeviceListModel(QStandardItemModel): 
    def flags(self, index):
        return Qt.ItemIsEnabled | Qt.ItemIsSelectable

def getDeviceList():
    return filter(lambda x : x.device_type == 'usb_device' and x.attributes.get('idVendor',None) == DEVICE_ID and x.attributes.get('idProduct',None) in [NET_ID, CON_ID], ctx.list_devices(subsystem='usb') )

ctx = pyudev.Context()
#for d in getDeviceList():
#    print help(d)
#for d in ctx.list_devices(subsystem='usb'):
#   print d.attributes.get('idVendor', None)


class Ui_DevMainWindow(Ui_MainWindow):
    lm = DeviceListModel()
    
    def setupUi(self, Window):
        Ui_MainWindow.setupUi(self,Window)
        Window.refresh.connect(self.do_refresh)
        self.Window = Window
        self.deviceList.setModel(self.lm)
        self.deviceList.selectionModel().currentChanged.connect(self.cfgStatusUpdate)
        self.cfgButton.clicked.connect(self.configure)
        self.actionConfigure.activated.connect(self.configure)
        self.do_refresh()
        self.monitor = pyudev.Monitor.from_netlink(ctx)
        self.monitor.filter_by(subsystem='usb')
        self.observer = QUDevMonitorObserver(self.monitor)
        self.observer.deviceAdded.connect(self.do_refresh)
        self.observer.deviceRemoved.connect(self.do_refresh)
        self.monitor.start()
       
    def do_refresh(self):
        self.cfgButton.setEnabled(False)
        self.actionConfigure.setEnabled(False)
        self.lm.clear()
        self.lm.setHorizontalHeaderLabels(["Device path", "Type", "Interface"])
        for d in getDeviceList():
            iface = ''
            dev_node = ''
            if d.attributes['idProduct'] == NET_ID:
                iface = list(list(d.children)[0].children)[0].device_path.rpartition('/')[2]
            if d.attributes['idProduct'] == CON_ID:
                if len(list(d.children)) < 1 or len(list(list(d.children)[0].children)) < 1 or len(list(list(list(d.children)[0].children)[0].children)) < 1:
                    dev_node = 'Driver not installed'
                else:
                    dev_node = list(list(list(d.children)[0].children)[0].children)[0].device_node
            self.lm.appendRow([QStandardItem(dev_node), QStandardItem('Configuration interface' if d.attributes['idProduct'] == CON_ID else 'VPN Network interface'), QStandardItem(iface)]);

    def cfgStatusUpdate(self, index):
        if self.lm.item(index.row(), 1).data(Qt.DisplayRole).toString() == 'Configuration interface':
            self.path = str(self.lm.item(index.row(), 0).data(Qt.DisplayRole).toString())
            print self.path
            self.cfgButton.setEnabled(True)
            self.actionConfigure.setEnabled(True)
        else:
            self.cfgButton.setEnabled(False)
            self.actionConfigure.setEnabled(False)

    def configure(self):
        configure.devcfg.Configure(self.path, self.Window)

class DevMainWindow(QMainWindow):
    refresh = pyqtSignal()

        

app = QApplication(sys.argv)
mainWindow = DevMainWindow()
mainUi = Ui_DevMainWindow()
mainUi.setupUi(mainWindow)
mainWindow.show()



sys.exit(app.exec_())
