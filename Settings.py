import sys
import subprocess
import re
import random
import os
import webbrowser

from PyQt6.QtWidgets import (
    QApplication, QLabel, QPushButton, QWidget, QTextEdit,
    QComboBox, QMessageBox, QSlider, QLineEdit
)
from PyQt6.QtGui import QPixmap, QFont, QIcon, QTransform
from PyQt6.QtCore import Qt, QPoint, QTimer, QSize, QPropertyAnimation, QEasingCurve, QUrl
from PyQt6.QtMultimedia import QSoundEffect

from components.about import AboutManager
from components.display import DisplayManager
from components.network import NetworkManager
from components.sound import SoundManager
from components.applications import ApplicationsManager
from components.wallpaper import WallpaperManager
from components.bluetooth import BluetoothManager
from components.battery import BatteryManager
from components.power import PowerManager
from components.storage import StorageManager

ASSETS_PATH = os.path.expanduser("~/Elysia/assets")

def strip_ansi(text):
    ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
    return ansi_escape.sub('', text)


class AnimatedTile(QPushButton):
    def __init__(self, parent, img_path, label_text, tile_pos, label_pos, effect_type="rotate", click_handler=None, is_car=False):
        super().__init__(parent)
        self.parent = parent
        self.effect_type = effect_type
        self.base_pos = QPoint(*tile_pos)
        self.setMouseTracking(True)
        self.is_destroyed = False
        self.is_car = is_car

        full_img_path = os.path.join(ASSETS_PATH, img_path)
        self.pixmap = QPixmap(full_img_path).scaled(130, 250, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
        self.original_pixmap = self.pixmap
        self.setIcon(QIcon(self.pixmap))
        self.setIconSize(self.pixmap.size())
        self.resize(self.pixmap.size())
        self.setStyleSheet("border: none; background: transparent;")
        self.move(self.base_pos)

        self.glass = QLabel(parent)
        self.glass.setGeometry(self.geometry())
        self.glass.setStyleSheet("background-color: rgba(255, 255, 255, 0.08); border-radius: 20px;")
        self.glass.lower()

        self.label = QLabel(label_text, parent)
        self.label.setFont(QFont("hoyofont", 14, QFont.Weight.Bold))
        self.label.setStyleSheet("color: pink; background: transparent;")
        self.label.adjustSize()
        self.label.move(*label_pos)

        self.anim_timer = QTimer()
        self.anim_timer.setInterval(16)
        self.anim_timer.timeout.connect(self.update_effect)
        self.mouse_offset = QPoint(0, 0)

        self.bounce_dir = 1
        self.bounce_offset = 0
        self.fall_animation = None

        if click_handler:
            self.clicked.connect(lambda: self.handle_click(click_handler))
        else:
            self.clicked.connect(self.handle_click)

    def handle_click(self, handler=None):
        if handler:
            handler()

    def enterEvent(self, event):
        if not self.is_destroyed:
            self.anim_timer.start()
            self.bounce_dir = 1
        return super().enterEvent(event)

    def leaveEvent(self, event):
        if not self.is_destroyed:
            self.anim_timer.stop()
            self.setIcon(QIcon(self.original_pixmap))
            self.setIconSize(self.original_pixmap.size())
            self.resize(self.original_pixmap.size())
            self.move(self.base_pos)
            self.glass.setGeometry(self.geometry())
        return super().leaveEvent(event)

    def mouseMoveEvent(self, event):
        if not self.is_destroyed:
            self.mouse_offset = event.position().toPoint()
        return super().mouseMoveEvent(event)

    def update_effect(self):
        if self.is_destroyed:
            return

        if self.effect_type == "rotate":
            angle = (self.mouse_offset.x() - self.width() / 2) * 0.1
            transform = QTransform().rotate(angle)
            rotated = self.original_pixmap.transformed(transform, Qt.TransformationMode.SmoothTransformation)
            self.setIcon(QIcon(rotated))
            self.setIconSize(rotated.size())

        self.bounce_offset += self.bounce_dir * 0.7
        if self.bounce_offset > 8 or self.bounce_offset < -8:
            self.bounce_dir *= -1

        new_y = self.base_pos.y() + self.bounce_offset
        self.move(self.base_pos.x(), int(new_y))
        self.glass.move(self.x(), self.y())

    def destroy_with_fall(self):
        if self.is_destroyed:
            return

        self.is_destroyed = True
        self.anim_timer.stop()

        self.fall_animation = QPropertyAnimation(self, b"pos")
        self.fall_animation.setDuration(1200)
        self.fall_animation.setStartValue(self.pos())
        end_x = self.x() + random.randint(-100, 100)
        end_y = self.parent.height() + 100
        self.fall_animation.setEndValue(QPoint(end_x, end_y))
        self.fall_animation.setEasingCurve(QEasingCurve.Type.InQuad)
        self.fall_animation.start()

        self.fade_timer = QTimer()
        self.fade_timer.setInterval(30)
        self.opacity = 1.0
        self.fade_timer.timeout.connect(self.fade_out)
        self.fade_timer.start()

        self.glass.hide()
        self.label.hide()

    def fade_out(self):
        self.opacity -= 0.03
        if self.opacity <= 0:
            self.fade_timer.stop()
            self.hide()
            return
        self.setStyleSheet(f"border: none; background: transparent; opacity: {self.opacity};")


class SettingsWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Elysia Settings")
        self.setFixedSize(1600, 800)
        self.setMouseTracking(True)

        self.bg_label = QLabel(self)
        self.bg_label.setPixmap(QPixmap(os.path.join(ASSETS_PATH, "assets/background.png")).scaled(
            self.size(), Qt.AspectRatioMode.IgnoreAspectRatio, Qt.TransformationMode.SmoothTransformation))
        self.bg_label.setGeometry(0, 0, self.width(), self.height())

        self.tiles = []
        self.current_view = None
        self.destruction_active = False

        self.init_tiles()
        self.init_back_button()

        self.about_manager = AboutManager(self)
        self.display_manager = DisplayManager(self)
        self.network_manager = NetworkManager(self)
        self.sound_manager = SoundManager(self)
        self.applications_manager = ApplicationsManager(self)
        self.wallpaper_manager = WallpaperManager(self)
        self.bluetooth_manager = BluetoothManager(self)
        self.battery_manager = BatteryManager(self)
        self.power_manager = PowerManager(self)
        self.storage_manager = StorageManager(self)

    def init_back_button(self):
        self.back_btn = QPushButton(self)
        self.back_btn.setIcon(QIcon(os.path.join(ASSETS_PATH, "assets/back.png")))
        self.back_btn.setIconSize(QSize(159, 66))
        self.back_btn.setFixedSize(159, 66)
        self.back_btn.setVisible(False)
        self.back_btn.setStyleSheet("border: none; background: transparent;")
        self.back_btn.move(20, 2)
        self.back_btn.clicked.connect(self.handle_back_click)

        self.back_label = QLabel("BACK", self)
        self.back_label.setFont(QFont("hoyofont", 18, QFont.Weight.Bold))
        self.back_label.setStyleSheet("color: white; background: transparent;")
        self.back_label.adjustSize()
        self.back_label.move(self.back_btn.x() + self.back_btn.width() - 125,
                             self.back_btn.y() + self.back_btn.height() // 2 - self.back_label.height() // 2)
        self.back_label.setVisible(False)

    def handle_back_click(self):
        if self.current_view:
            getattr(self, f"{self.current_view}_manager").hide()
            self.current_view = None

    def init_tiles(self):
        def add_tile(img, text, tile_pos, label_pos, handler=None, is_car=False):
            tile = AnimatedTile(self, os.path.join("assets", img), text, tile_pos, label_pos, click_handler=handler, is_car=is_car)
            self.tiles.append(tile)

        add_tile("about.png", "ABOUT", (750, 240), (764, 210), self.show_about)
        add_tile("display.png", "DISPLAY", (880, 130), (885, 115), self.show_display)
        add_tile("network.png", "NETWORK", (940, 380), (930, 370), self.show_network)
        add_tile("update.png", "UPDATES", (1080, 183), (1075, 180), self.show_update)
        add_tile("snake.png", "STORAGE", (1195, 340), (1200, 330), self.show_storage)
        add_tile("apps.png", "APPS", (1290, 162), (1300, 160), self.show_applications)
        add_tile("support.png", "SUPPORT", (1340, 410), (1330, 405), self.show_support)
        add_tile("bluetooth.png", "BLUETOOTH", (630, 95), (620, 100), self.show_bluetooth)
        add_tile("sound.png", "SOUND", (610, 395), (625, 380), self.show_sound)
        add_tile("power.png", "POWER", (480, 210), (495, 205), self.show_power)
        add_tile("battery.png", "BATTERY", (325, 105), (337, 100), self.show_battery)
        add_tile("wallpaper.png", "APPEARANCE", (366, 355), (349, 340), self.show_wallpaper)
        add_tile("car.png", "CAR", (190, 290), (225, 290), self.trigger_destruction, is_car=True)

    def show_about(self): self._show_section("about")
    def show_display(self): self._show_section("display")
    def show_network(self): self._show_section("network")
    def show_sound(self): self._show_section("sound")
    def show_applications(self): self._show_section("applications")
    def show_wallpaper(self): self._show_section("wallpaper")
    def show_bluetooth(self): self._show_section("bluetooth")
    def show_battery(self): self._show_section("battery")
    def show_power(self): self._show_section("power")
    def show_storage(self): self._show_section("storage")

    def _show_section(self, name):
        if not self.destruction_active:
            self.current_view = name
            getattr(self, f"{name}_manager").show()

    def show_support(self):
        if not self.destruction_active:
            webbrowser.open("https://ko-fi.com/matsuko3")

    def show_update(self):
        if not self.destruction_active:
            try:
                subprocess.Popen([os.path.expanduser("elysia-updater.sh")])
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to run update script:\n{e}")

    def trigger_destruction(self):
        if self.destruction_active: return
        self.destruction_active = True
        for i, tile in enumerate(self.tiles):
            QTimer.singleShot(i * 150, tile.destroy_with_fall)
        QTimer.singleShot(len(self.tiles) * 150 + 2000, self.close_application)

    def close_application(self):
        QApplication.instance().quit()

    def hide_tiles(self):
        for tile in self.tiles:
            tile.hide(); tile.glass.hide(); tile.label.hide()

    def show_tiles(self):
        for tile in self.tiles:
            if not tile.is_destroyed:
                tile.show(); tile.glass.show(); tile.label.show()

    def show_back_button(self): self.back_btn.setVisible(True); self.back_label.setVisible(True)
    def hide_back_button(self): self.back_btn.setVisible(False); self.back_label.setVisible(False)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = SettingsWindow()
    win.show()
    sys.exit(app.exec())
