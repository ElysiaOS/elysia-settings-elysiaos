import os
import re
from PyQt6.QtWidgets import (
    QLabel, QTextEdit, QVBoxLayout, QScrollArea, QWidget, QPushButton,
    QHBoxLayout, QLineEdit, QDialog, QDialogButtonBox, QFormLayout,
    QListWidget, QMessageBox, QCheckBox
)
from PyQt6.QtGui import QFont
from PyQt6.QtCore import Qt, QTimer


class ApplicationsManager:
    def __init__(self, parent):
        self.parent = parent
        self.init_ui()

        # Auto-refresh every 1000ms
        self.timer = QTimer()
        self.timer.timeout.connect(self.refresh_data)
        self.timer.start(5000)

    def init_ui(self):
        self.container = QWidget(self.parent)
        self.container.setGeometry(80, 80, 1440, 640)

        self.container.setStyleSheet("""
            background-color: rgba(255, 255, 255, 0.05);
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
        """)
        self.container.setWindowFlag(Qt.WindowType.WindowStaysOnTopHint, True)
        self.container.setVisible(False)

        layout = QVBoxLayout(self.container)
        layout.setContentsMargins(40, 40, 40, 40)
        layout.setSpacing(40)

        self.text_font = QFont("monospace", 10)
        self.text_font.setStyleHint(QFont.StyleHint.Monospace)

        button_bar = QHBoxLayout()
        add_btn = QPushButton("+ APP")
        remove_btn = QPushButton("- APP")
        add_keybind_btn = QPushButton("+ KB")
        remove_keybind_btn = QPushButton("- KB")

        for btn in (add_btn, remove_btn, add_keybind_btn, remove_keybind_btn):
            btn.setFixedSize(70, 60)
            btn.setStyleSheet("""
                QPushButton {
                    color: white;
                    background-color: rgba(255, 255, 255, 0.05);
                    border: 2px solid rgba(255,255,255,0.3);
                    border-radius: 18px;
                    font-size: 18px;
                    font-weight: bold;
                }
                QPushButton:hover {
                    background-color: rgba(110, 110, 110, 1);
                }
            """)

        add_btn.clicked.connect(self.open_add_dialog)
        remove_btn.clicked.connect(self.open_remove_dialog)
        add_keybind_btn.clicked.connect(self.open_add_keybind_dialog)
        remove_keybind_btn.clicked.connect(self.open_remove_keybind_dialog)

        button_bar.addWidget(add_btn)
        button_bar.addWidget(remove_btn)
        button_bar.addSpacing(20)
        button_bar.addWidget(add_keybind_btn)
        button_bar.addWidget(remove_keybind_btn)
        button_bar.addStretch()
        layout.addLayout(button_bar)

        self.applications_box = self.create_scroll_area("Applications", self.load_applications())
        self.keybinds_box = self.create_scroll_area("Keybinds", self.load_keybinds())

        layout.addWidget(self.applications_box)
        layout.addWidget(self.keybinds_box)

    def refresh_data(self):
        self.applications_box.text_edit.setText(self.load_applications())
        self.keybinds_box.text_edit.setText(self.load_keybinds())

    def create_scroll_area(self, title, content):
        label = QLabel(f"{title.upper()}")
        label.setFont(QFont("ElysiaOSNew", 16, QFont.Weight.Bold))
        label.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")
        label.setGeometry(650, 45, 300, 635)
        label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        text_edit = QTextEdit()
        text_edit.setFont(self.text_font)
        text_edit.setText(content)
        text_edit.setReadOnly(True)
        text_edit.setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px")

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        container = QWidget()
        vbox = QVBoxLayout(container)
        vbox.setContentsMargins(0, 0, 0, 0)
        vbox.setSpacing(5)
        vbox.addWidget(label)
        vbox.addWidget(text_edit)
        scroll.setWidget(container)
        scroll.setStyleSheet("QScrollArea { background-color: transparent; border: none; }")

        scroll.text_edit = text_edit
        return scroll

    def show(self):
        self.container.setVisible(True)
        self.parent.hide_tiles()
        self.parent.show_back_button()
        self.parent.current_view = "applications"

    def hide(self):
        self.container.setVisible(False)
        self.parent.show_tiles()
        self.parent.hide_back_button()

    def load_applications(self):
        path = os.path.expanduser("~/.config/hypr/applications.conf")
        if not os.path.exists(path):
            return "No applications.conf found."

        self.app_lines = []
        display_lines = []
        with open(path, "r") as f:
            for line in f:
                stripped = line.strip()
                self.app_lines.append(line.rstrip("\n"))
                if stripped.startswith("#") or not stripped:
                    continue
                if "=" in stripped:
                    key, cmd = map(str.strip, stripped.split("=", 1))
                    key_name = key.replace("$", "").capitalize()
                    display_lines.append(f"{key_name:<30} {cmd}")
        return "\n".join(display_lines[:30])

    def load_keybinds(self):
        path = os.path.expanduser("~/.config/hypr/app_keybinds.conf")
        if not os.path.exists(path):
            return "No app_keybinds.conf found."

        variables = {}
        binds = []

        with open(path, "r") as f:
            for line in f:
                line = line.strip()
                if line.startswith("$") and "=" in line:
                    var, val = map(str.strip, line.split("=", 1))
                    variables[var] = val

        bind_count = 1
        with open(path, "r") as f:
            for line in f:
                original = line.strip()
                if not original.startswith("bind"):
                    continue

                line = re.sub(r"#.*", "", original).strip()
                if "=" in line:
                    line = line.split("=", 1)[1].strip()

                parts = [p.strip() for p in line.split(",")]
                if len(parts) >= 3:
                    mod, key, action_type = parts[:3]
                    action = ",".join(parts[3:]).strip() if len(parts) > 3 else ""

                    for var, val in variables.items():
                        mod = mod.replace(var, val)
                        action = action.replace(var, val)

                    if action_type == "exec":
                        keybind_str = f"BIND {bind_count:<3} {mod} KEY + {key:<6} {action}"
                    else:
                        keybind_str = f"BIND {bind_count:<3} {mod} KEY + {key:<6} {action_type}"
                    binds.append(keybind_str)
                    bind_count += 1

        return "\n".join(binds) if binds else "No keybinds found."

    # --- Remaining dialog code is unchanged (add/remove application/keybinds) ---
    # It's all retained from your original code, ensuring full functionality.
    # If needed, I can re-include that as well.

    # Keep using self.refresh_data() after any edit operations


    def open_add_dialog(self):
        dialog = QDialog(self.container)
        dialog.setWindowTitle("Add Application")
        dialog.setFixedSize(400, 180)
        dialog.setStyleSheet("""
            background-color: #202020;
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
            color: white;
            font-family: 'ElysiaOSNew';
            font-size: 14px;
        """)

        form = QFormLayout(dialog)
        app_name_input = QLineEdit()
        app_exec_input = QLineEdit()
        form.addRow("Application Name:", app_name_input)
        form.addRow("Application Exec:", app_exec_input)

        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(lambda: self.add_application(dialog, app_name_input.text(), app_exec_input.text()))
        buttons.rejected.connect(dialog.reject)
        form.addRow(buttons)

        dialog.exec()

    def add_application(self, dialog, name, exec_path):
        name = name.strip()
        exec_path = exec_path.strip()

        if not name or not exec_path:
            QMessageBox.warning(dialog, "Input Error", "Both Application Name and Exec path are required.")
            return

        conf_path = os.path.expanduser("~/.config/hypr/applications.conf")
        with open(conf_path, "a") as f:
            f.write(f"\n${name} = {exec_path}\n")

        updated_content = self.load_applications()
        self.applications_box.text_edit.setText(updated_content)
        dialog.accept()

    def open_remove_dialog(self):
        dialog = QDialog(self.container)
        dialog.setWindowTitle("Remove Application")
        dialog.setFixedSize(500, 400)
        dialog.setStyleSheet("""
            background-color: #202020;
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
            color: white;
            font-family: 'ElysiaOSNew', monospace;
            font-size: 14px;
        """)

        vbox = QVBoxLayout(dialog)
        list_widget = QListWidget()
        deletable = []

        for line in self.app_lines:
            if line.strip().startswith("$") and "=" in line:
                deletable.append(line)
                key, cmd = map(str.strip, line.split("=", 1))
                key_name = key.replace("$", "").capitalize()
                list_widget.addItem(f"{key_name:<30} {cmd}")

        vbox.addWidget(list_widget)

        buttons = QDialogButtonBox()
        apply_btn = buttons.addButton("Apply Changes", QDialogButtonBox.ButtonRole.AcceptRole)
        cancel_btn = buttons.addButton("Cancel", QDialogButtonBox.ButtonRole.RejectRole)
        vbox.addWidget(buttons)

        def apply_removal():
            selected = list_widget.currentRow()
            if selected < 0:
                QMessageBox.warning(dialog, "No selection", "Please select an entry to remove.")
                return

            to_remove = deletable[selected]
            self.app_lines.remove(to_remove)

            path = os.path.expanduser("~/.config/hypr/applications.conf")
            with open(path, "w") as f:
                for line in self.app_lines:
                    f.write(line + "\n")

            updated_content = self.load_applications()
            self.applications_box.text_edit.setText(updated_content)
            dialog.accept()

        apply_btn.clicked.connect(apply_removal)
        cancel_btn.clicked.connect(dialog.reject)
        dialog.exec()

    # Keybind dialogs and functions

    def open_add_keybind_dialog(self):
        dialog = QDialog(self.container)
        dialog.setWindowTitle("Add Keybind")
        dialog.setFixedSize(400, 260)
        dialog.setStyleSheet("""
            background-color: #202020;
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
            color: white;
            font-family: 'ElysiaOSNew', monospace;
            font-size: 14px;
        """)

        form = QFormLayout(dialog)
        self.keybind1_input = QLineEdit()
        self.keybind2_input = QLineEdit()
        self.keybind3_input = QLineEdit()
        self.exec_checkbox = QCheckBox("Exec")
        self.application_call_input = QLineEdit()

        form.addRow("Keybind1 (e.g. $mainMod):", self.keybind1_input)
        form.addRow("Keybind2 (e.g. L):", self.keybind2_input)
        form.addRow("Keybind3 (optional):", self.keybind3_input)
        form.addRow(self.exec_checkbox)
        form.addRow("Application Call:", self.application_call_input)

        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(lambda: self.add_keybind(dialog))
        buttons.rejected.connect(dialog.reject)
        form.addRow(buttons)

        dialog.exec()

    def add_keybind(self, dialog):
        kb1 = self.keybind1_input.text().strip()
        kb2 = self.keybind2_input.text().strip()
        kb3 = self.keybind3_input.text().strip()
        is_exec = self.exec_checkbox.isChecked()
        app_call = self.application_call_input.text().strip()

        if not kb1 or not kb2 or not app_call:
            QMessageBox.warning(dialog, "Input Error", "Keybind1, Keybind2, and Application Call are required.")
            return

        parts = [kb1, kb2]
        if kb3:
            parts.append(kb3)

        if is_exec:
            bind_line = f"bind = {', '.join(parts)}, exec, {app_call}"
        else:
            bind_line = f"bind = {', '.join(parts)}, {app_call}"

        conf_path = os.path.expanduser("~/.config/hypr/app_keybinds.conf")
        with open(conf_path, "a") as f:
            f.write("\n" + bind_line + "\n")

        updated_content = self.load_keybinds()
        self.keybinds_box.text_edit.setText(updated_content)
        dialog.accept()

    def open_remove_keybind_dialog(self):
        dialog = QDialog(self.container)
        dialog.setWindowTitle("Remove Keybind")
        dialog.setFixedSize(600, 400)
        dialog.setStyleSheet("""
            background-color: #202020;
            border: 2px solid rgba(255, 255, 255, 0.15);
            border-radius: 30px;
            color: white;
            font-family: 'ElysiaOSNew', monospace;
            font-size: 14px;
        """)

        vbox = QVBoxLayout(dialog)
        list_widget = QListWidget()
        deletable = []

        path = os.path.expanduser("~/.config/hypr/app_keybinds.conf")
        if not os.path.exists(path):
            QMessageBox.information(dialog, "Info", "No keybind config found.")
            dialog.reject()
            return

        with open(path, "r") as f:
            for line in f:
                line_strip = line.strip()
                if line_strip.startswith("bind"):
                    deletable.append(line_strip)
                    list_widget.addItem(line_strip)

        vbox.addWidget(list_widget)

        buttons = QDialogButtonBox()
        apply_btn = buttons.addButton("Apply Changes", QDialogButtonBox.ButtonRole.AcceptRole)
        cancel_btn = buttons.addButton("Cancel", QDialogButtonBox.ButtonRole.RejectRole)
        vbox.addWidget(buttons)

        def apply_removal():
            selected = list_widget.currentRow()
            if selected < 0:
                QMessageBox.warning(dialog, "No selection", "Please select a keybind to remove.")
                return

            to_remove = deletable[selected]
            deletable.pop(selected)

            with open(path, "r") as f_read:
                lines = f_read.readlines()

            with open(path, "w") as f_write:
                for line in lines:
                    if line.strip() != to_remove:
                        f_write.write(line)

            updated_content = self.load_keybinds()
            self.keybinds_box.text_edit.setText(updated_content)
            dialog.accept()

        apply_btn.clicked.connect(apply_removal)
        cancel_btn.clicked.connect(dialog.reject)
        dialog.exec()
