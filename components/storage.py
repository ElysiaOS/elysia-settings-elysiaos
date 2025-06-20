import os
import shutil
import psutil
from PyQt6.QtWidgets import (
    QLabel, QWidget, QVBoxLayout, QHBoxLayout, QProgressBar,
    QFrame, QSizePolicy
)
from PyQt6.QtGui import QPixmap, QFont
from PyQt6.QtCore import Qt, QTimer


class StorageManager:
    def __init__(self, parent):
        self.parent = parent
        self.drive_widgets = []

        self.init_ui()
        self.setup_timer()

    def init_ui(self):
        self.storage_box = QWidget(self.parent)
        self.storage_box.setGeometry(455, 100, 700, 590)
        self.storage_box.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.storage_box.setVisible(False)

        self.layout = QVBoxLayout(self.storage_box)
        self.layout.setContentsMargins(60, 60, 60, 60)
        self.layout.setSpacing(20)

        storage_label = QLabel("STORAGE", self.storage_box)
        storage_label.setFont(QFont("ElysiaOSNew", 16))
        storage_label.setStyleSheet("color: white; background: transparent; border: none;")
        storage_label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.layout.addWidget(storage_label)

        self.drives_container = QVBoxLayout()
        self.drives_container.setSpacing(20)
        self.layout.addLayout(self.drives_container)

    def show(self):
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.storage_box.setVisible(True)
        self.update_disks()

    def hide(self):
        self.storage_box.setVisible(False)
        self.parent.hide_back_button()
        self.parent.show_tiles()

    def setup_timer(self):
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_disks)
        self.timer.start(1000)  # every 1000ms

    def update_disks(self):
        # Clear previous widgets
        for widget in self.drive_widgets:
            widget.setParent(None)
        self.drive_widgets.clear()

        partitions = psutil.disk_partitions()

        for part in partitions:
            try:
                usage = shutil.disk_usage(part.mountpoint)
                total = usage.total
                free = usage.free
                used = total - free
                percent = int(used / total * 100)
            except Exception:
                continue

            drive_frame = QFrame()
            drive_layout = QHBoxLayout(drive_frame)
            drive_layout.setSpacing(15)
            drive_layout.setContentsMargins(10, 10, 10, 10)

            # Drive icon
            icon_label = QLabel()
            icon_path = os.path.expanduser("~/Elysia/assets/assets/drive.png")
            if os.path.exists(icon_path):
                pixmap = QPixmap(icon_path).scaled(60, 60, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
                icon_label.setPixmap(pixmap)
            else:
                icon_label.setText("Missing")
            icon_label.setFixedSize(50, 50)
            icon_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
            drive_layout.addWidget(icon_label)

            # Info section
            info_container = QVBoxLayout()
            info_container.setSpacing(5)

            drive_label = QLabel(f"{part.device} ({part.mountpoint})")
            drive_label.setFont(QFont("ElysiaOSNew", 11))
            drive_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")

            size_label = QLabel(f"{self.bytes_to_gb(used)} used / {self.bytes_to_gb(total)} total")
            size_label.setFont(QFont("ElysiaOSNew", 10))
            size_label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")

            progress = QProgressBar()
            progress.setMinimum(0)
            progress.setMaximum(100)
            progress.setValue(percent)
            progress.setTextVisible(True)
            progress.setFormat(f"{self.bytes_to_gb(free)} free")
            progress.setFixedHeight(20)
            progress.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
            progress.setStyleSheet("""
                QProgressBar {
                    border: 2px solid rgba(255, 255, 255, 0.1);
                    border-radius: 10px;
                    background-color: rgba(255, 255, 255, 0.05);
                    color: white;
                    text-align: center;
                }
                QProgressBar::chunk {
                    background-color: #e5a7c6;
                    border-radius: 10px;
                }
            """)

            info_container.addWidget(drive_label)
            info_container.addWidget(size_label)
            info_container.addWidget(progress)

            drive_layout.addLayout(info_container)
            self.drives_container.addWidget(drive_frame)
            self.drive_widgets.append(drive_frame)

    def bytes_to_gb(self, bytes_val):
        return f"{bytes_val / (1024 ** 3):.1f} GB"
