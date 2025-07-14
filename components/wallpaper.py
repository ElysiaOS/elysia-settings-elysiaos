import os
import subprocess
from PyQt6.QtWidgets import (
    QLabel, QWidget, QVBoxLayout, QHBoxLayout, QPushButton,
    QFrame, QStackedLayout
)
from PyQt6.QtGui import QPixmap, QFont
from PyQt6.QtCore import Qt, QThread, pyqtSignal, QTimer


class WallpaperLoader(QThread):
    loaded = pyqtSignal(list)

    def __init__(self, folder):
        super().__init__()
        self.folder = folder

    def run(self):
        wallpapers = [
            f for f in sorted(os.listdir(self.folder))
            if f.lower().endswith((".jpg", ".png"))
        ] if os.path.exists(self.folder) else []
        self.loaded.emit(wallpapers)


class WallpaperManager:
    def __init__(self, parent):
        self.parent = parent
        self.wallpapers = []
        self.current_index = 0
        self.theme_selected = None
        self.wallpaper_selected = None
        self.theme_boxes = {}
        self.loading_thread = None
        self.last_detected_theme = None

        self.theme_timer = QTimer()
        self.theme_timer.timeout.connect(self.check_theme_update)

        self.init_ui()

    def init_ui(self):
        self.wallpaper_box = QWidget(self.parent)
        self.wallpaper_box.setGeometry(455, 100, 700, 650)
        self.wallpaper_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.wallpaper_box.setVisible(False)

        layout = QVBoxLayout(self.wallpaper_box)
        layout.setContentsMargins(150, 100, 150, 400)
        layout.setSpacing(25)

        theme_label = QLabel("Theme", self.wallpaper_box)
        theme_label.setFont(QFont("ElysiaOSNew", 16))
        theme_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        theme_label.setGeometry(315, 0, 500, 160)

        theme_widget = QWidget(self.wallpaper_box)
        theme_widget.setGeometry(110, 320, 110, 250)
        theme_container = QHBoxLayout(theme_widget)
        layout.addWidget(theme_widget)

        for theme in ["Light", "Dark"]:
            frame = QFrame(self.wallpaper_box)
            frame.setFixedSize(130, 130)
            frame.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            frame.setCursor(Qt.CursorShape.PointingHandCursor)

            stacked_layout = QStackedLayout(frame)
            image_label = QLabel()
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)

            image_path = os.path.expanduser(f"~/Elysia/assets/assets/{theme.lower()}.png")
            if os.path.exists(image_path):
                pixmap = QPixmap(image_path).scaled(250, 250, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
                image_label.setPixmap(pixmap)
            else:
                image_label.setText("Missing")

            tick = QLabel("✔")
            tick.setStyleSheet("color: #a87c4f; font-size: 24px; background: transparent;")
            tick.setAlignment(Qt.AlignmentFlag.AlignTop | Qt.AlignmentFlag.AlignRight)
            tick.setVisible(False)

            stacked_layout.addWidget(image_label)
            stacked_layout.addWidget(tick)

            frame.mousePressEvent = lambda _, t=theme: self.select_theme(t)
            self.theme_boxes[theme] = (frame, tick)
            theme_container.addWidget(frame)

        layout.addLayout(theme_container)

        wallpaper_label = QLabel("Wallpaper", self.wallpaper_box)
        wallpaper_label.setFont(QFont("ElysiaOSNew", 16))
        wallpaper_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        wallpaper_label.setGeometry(295, 220, 200, 160)

        nav_box = QHBoxLayout()
        nav_box.setSpacing(20)
        nav_box.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.left_btn = QPushButton("⮜", self.wallpaper_box)
        self.left_btn.setFixedSize(40, 40)
        self.left_btn.clicked.connect(self.previous_wallpaper)
        self.left_btn.setGeometry(50, 390, 0, 250)
        self.left_btn.setStyleSheet("color: white; font-size: 26px; background: transparent; border: none;")

        self.wallpaper_preview = QLabel("", self.wallpaper_box)
        self.wallpaper_preview.setFixedSize(500, 200)
        self.wallpaper_preview.setGeometry(110, 320, 0, 250)
        self.wallpaper_preview.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")

        self.right_btn = QPushButton("⮞", self.wallpaper_box)
        self.right_btn.setFixedSize(40, 40)
        self.right_btn.clicked.connect(self.next_wallpaper)
        self.right_btn.setGeometry(625, 390, 0, 250)
        self.right_btn.setStyleSheet("color: white; font-size: 26px; background: transparent; border: none;")

        layout.addLayout(nav_box)

        self.apply_btn = QPushButton("Apply Wallpaper", self.wallpaper_box)
        self.apply_btn.setFixedSize(200, 50)
        self.apply_btn.setStyleSheet("""
            QPushButton {
                background-color: #e5a7c6;
                color: white;
                font-weight: bold;
                border: none;
                padding: 6px 12px;
                letter-spacing: 1px;
                border-radius: 10px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #edcee3;
            }
        """)
        self.apply_btn.clicked.connect(self.apply_changes)
        self.apply_btn.setGeometry(370, 570, 200, 150)

        self.browse_btn = QPushButton("Browse Wallpapers", self.wallpaper_box)
        self.browse_btn.setFixedSize(200, 50)
        self.browse_btn.setStyleSheet("""
            QPushButton {
                background-color: #e5a7c6;
                color: white;
                font-weight: bold;
                border: none;
                padding: 6px 12px;
                letter-spacing: 1px;
                border-radius: 10px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #edcee3;
            }
        """)
        self.browse_btn.clicked.connect(self.open_wallpaper_folder)
        self.browse_btn.setGeometry(150, 570, 500, 50)

    def open_wallpaper_folder(self):
        path = os.path.expanduser("~/Elysia/wallpaper")
        subprocess.Popen(["xdg-open", path])

    def show(self):
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.wallpaper_box.setVisible(True)
        self.last_detected_theme = self.detect_theme()
        self.select_theme(self.last_detected_theme)
        self.theme_timer.start(1000)

    def hide(self):
        self.wallpaper_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()
        self.theme_timer.stop()

    def detect_theme(self):
        try:
            out = subprocess.check_output([
                "gsettings", "get", "org.gnome.desktop.interface", "color-scheme"
            ]).decode()
            return "Light" if "light" in out.lower() else "Dark"
        except Exception:
            return "Light"

    def check_theme_update(self):
        new_theme = self.detect_theme()
        if new_theme != self.last_detected_theme:
            self.last_detected_theme = new_theme
            self.select_theme(new_theme)

    def select_theme(self, theme):
        if self.theme_selected == theme:
            self.load_wallpapers()
            return

        self.theme_selected = theme
        for name, (frame, tick) in self.theme_boxes.items():
            is_selected = name == theme
            frame.setStyleSheet(f"""
                border: {'3px solid #e5a7c6' if is_selected else '2px solid transparent'};
                border-radius: 26px;
                background: transparent;
            """)
            tick.setVisible(is_selected)

        self.load_wallpapers()

        script_path = os.path.expanduser(f"~/Elysia/{theme}Theme.sh")
        if os.path.exists(script_path):
            subprocess.Popen([script_path])

    def load_wallpapers(self):
        folder = os.path.expanduser(f"~/Elysia/wallpaper/{self.theme_selected}/")
        if not os.path.exists(folder):
            self.wallpapers = []
            self.wallpaper_preview.clear()
            return

        self.loading_thread = WallpaperLoader(folder)
        self.loading_thread.loaded.connect(self.display_wallpapers)
        self.loading_thread.start()

    def display_wallpapers(self, wallpapers):
        self.wallpapers = wallpapers
        self.current_index = 0
        self.update_wallpaper_preview()

    def update_wallpaper_preview(self):
        if not self.wallpapers:
            self.wallpaper_preview.clear()
            return

        self.wallpaper_selected = self.wallpapers[self.current_index]
        path = os.path.expanduser(f"~/Elysia/wallpaper/{self.theme_selected}/{self.wallpaper_selected}")
        if os.path.exists(path):
            pix = QPixmap(path).scaled(
                500, 280, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation
            )
            self.wallpaper_preview.setPixmap(pix)

    def previous_wallpaper(self):
        if self.wallpapers:
            self.current_index = (self.current_index - 1) % len(self.wallpapers)
            self.update_wallpaper_preview()

    def next_wallpaper(self):
        if self.wallpapers:
            self.current_index = (self.current_index + 1) % len(self.wallpapers)
            self.update_wallpaper_preview()

    def apply_changes(self):
        if not self.wallpaper_selected:
            return

        path = os.path.expanduser(f"~/Elysia/wallpaper/{self.theme_selected}/{self.wallpaper_selected}")
        if os.path.exists(path):
            subprocess.Popen([
                "swww", "img", path,
                "--transition-type", "grow",
                "--transition-step", "10",
                "--transition-fps", "60"
            ])
