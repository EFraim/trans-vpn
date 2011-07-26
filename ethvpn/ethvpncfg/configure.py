from devcfg import Ui_devcfg
from PyQt4.QtCore import QAbstractListModel, QModelIndex, pyqtSignal, Qt, QVariant, QObject, SIGNAL, QString, pyqtSlot
from PyQt4.QtGui import QApplication, QDialog, QMainWindow, QStandardItemModel, QStandardItem
import time
import select

try:
    _fromUtf8 = QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s


class Callable:
    def __init__(self, anycallable):
        self.__call__ = anycallable

class devcfg(Ui_devcfg):
    def clearConnection(self):
        conn = open(self.path, 'w+b', 0)
        conn.write('\n\n')
        conn.flush()
        time.sleep(3)
        while len(select.select([conn],[],[],0.2)[0]) > 0:
            conn.read(1)
        return conn

    def readVal(self, conn):
        return conn.readline().split(' ')[1].strip()

    def loadText(self, conn, name, ctrl):
        conn.write("%s\n" % name)
        ctrl.setText(self.readVal(conn))
    
    def storeText(self, conn, name, ctrl):
        conn.write("%s %s\n" % (name, ctrl.text()))
        if conn.readline() != "OK\n":
            raise Exception("Saving %s failed"%(name))

    def storeTextArea(self, conn, name, ctrl):
        conn.write("%s %s\n" % (name, str(ctrl.document().toPlainText()).strip()))
        if conn.readline() != "OK\n":
            raise Exception("Saving %s failed"%(name))

    def Load(self):
        conn = self.clearConnection()
        self.loadText(conn, "vpnhost", self.vpnhostEdit)
        self.loadText(conn, "vpnport", self.vpnportEdit)
        self.loadText(conn, "vpnmac", self.vpnmacEdit)
        self.loadText(conn, "staddr", self.staddrEdit)
        self.loadText(conn, "stmask", self.stmaskEdit)
        self.loadText(conn, "stgw", self.stgwEdit)
        self.loadText(conn, "stdns", self.stdnsEdit)
        self.loadText(conn, "physmac", self.physmacEdit)
        self.loadText(conn, "servpubkey", self.servpubkeyEdit)
        self.loadText(conn, "clientpubkey", self.clientpubkeyEdit)
        self.loadText(conn, "clientprivkey", self.clientprivkeyEdit)
        conn.write("guestip\n")
        if self.readVal(conn) == "dhcp":
            self.staticEdit.setChecked(True)
            self.dhcpEdit.setChecked(True)
        else:
            self.dhcpEdit.setChecked(True)
            self.staticEdit.setChecked(True)
        conn.close()

    def Save(self):
        conn = self.clearConnection()
        self.storeText(conn, "vpnhost", self.vpnhostEdit)
        self.storeText(conn, "vpnport", self.vpnportEdit)
        self.storeText(conn, "vpnmac", self.vpnmacEdit)
        self.storeText(conn, "staddr", self.staddrEdit)
        self.storeText(conn, "stmask", self.stmaskEdit)
        self.storeText(conn, "stgw", self.stgwEdit)
        self.storeText(conn, "stdns", self.stdnsEdit)
        self.storeText(conn, "physmac", self.physmacEdit)
        self.storeTextArea(conn, "servpubkey", self.servpubkeyEdit)
        self.storeTextArea(conn, "clientpubkey", self.clientpubkeyEdit)
        self.storeTextArea(conn, "clientprivkey", self.clientprivkeyEdit)
        conn.write("guestip %s\n" % ("dhcp" if self.dhcpEdit.isChecked() else "static"))
        if conn.readline() != "OK\n":
            raise Exception("Saving guestip failed")
        conn.write("save\n")
        if conn.readline() != "OK\n":
            raise Exception("Writing to EEPROM failed")
        conn.close()

    def setupUi(self, dialog):
        Ui_devcfg.setupUi(self, dialog)
        #self.vpnhostEdit.textChanged.connect(self.Save)
        #self.buttonBox.accepted.connect(self.Save)
        self.buttonBox.accepted.connect(self.Save)
        print "UI setup done"
    
    def Configure(path, parent):
        wUi = devcfg()
        wUi.path = path
        w = QDialog(parent)
        wUi.setupUi(w)
        wUi.Load()
        #w.exec_()
        w.setModal(False)
        w.exec_()
    
    Configure = Callable(Configure)
