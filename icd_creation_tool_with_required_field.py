from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QLabel, QLineEdit, QPushButton, QVBoxLayout,
    QHBoxLayout, QStackedWidget, QListWidget, QTextEdit, QComboBox, QTableWidget, QTableWidgetItem,
    QHeaderView, QMessageBox
)
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt
import sys
import json
import requests  
import os

class LoginPage(QWidget):
    def __init__(self, parent):
        super().__init__()
        self.parent = parent

        self.setFixedSize(400, 300)
        self.setStyleSheet("""
            QWidget {
                background-color: #f0f0f5;
            }
            QLabel {
                font-size: 14px;
                color: #333333;
            }
            QLineEdit {
                padding: 5px;
                font-size: 14px;
                border: 1px solid #cccccc;
                border-radius: 5px;
                background-color: #ffffff;
            }
            QPushButton {
                font-size: 14px;
                padding: 5px 10px;
                background-color: #5cb85c;
                color: white;
                border-radius: 5px;
                border: none;
            }
            QPushButton:hover {
                background-color: #4cae4c;
            }
        """)

        self.layout = QVBoxLayout()

        title_label = QLabel("ICD Creation Tool Login")
        title_label.setFont(QFont("Arial", 16, QFont.Bold))
        title_label.setAlignment(Qt.AlignCenter)

        self.username_label = QLabel("Username:")
        self.username_input = QLineEdit()
        self.username_input.setPlaceholderText("Enter your username")

        self.password_label = QLabel("Password:")
        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        self.password_input.setPlaceholderText("Enter your password")

        self.login_button = QPushButton("Login")
        self.login_button.setFixedWidth(100)
        self.login_button.clicked.connect(self.login)

        self.register_button = QPushButton("Register")
        self.register_button.setFixedWidth(100)
        self.register_button.clicked.connect(self.register)

        button_layout = QHBoxLayout()
        button_layout.addWidget(self.login_button)
        button_layout.addWidget(self.register_button)
        button_layout.setAlignment(Qt.AlignCenter)

        self.layout.addWidget(title_label, alignment=Qt.AlignCenter)
        self.layout.addWidget(self.username_label, alignment=Qt.AlignCenter)
        self.layout.addWidget(self.username_input, alignment=Qt.AlignCenter)
        self.layout.addWidget(self.password_label, alignment=Qt.AlignCenter)
        self.layout.addWidget(self.password_input, alignment=Qt.AlignCenter)
        self.layout.addLayout(button_layout)

        self.layout.setAlignment(Qt.AlignCenter)
        self.setLayout(self.layout)

    def login(self):
        username = self.username_input.text()
        password = self.password_input.text()
        if username == "admin" and password == "admin":
            self.parent.switch_to_main()
        else:
            QMessageBox.warning(self, "Login Failed", "Invalid username or password")

    def register(self):
        QMessageBox.information(self, "Register", "Registration functionality.") #we have to implement

class MainPage(QWidget):
    def __init__(self):
        super().__init__()

        self.setStyleSheet("""
            QWidget {
                background-color: #e6e6ff;
            }
            QListWidget {
                font-size: 14px;
                border-right: 2px solid #cccccc;
                background-color: #ffffff;
            }
            QTableWidget {
                font-size: 12px;
                border: 1px solid #cccccc;
                border-radius: 5px;
                background-color: #ffffff;
            }
            QPushButton {
                font-size: 14px;
                padding: 5px 10px;
                background-color: #5cb85c;
                color: white;
                border-radius: 5px;
                border: none;
            }
            QPushButton:hover {
                background-color: #4cae4c;
            }
            QTextEdit {
                font-size: 12px;
                border: 1px solid #cccccc;
                border-radius: 5px;
                background-color: #ffffff;
            }
            QLabel.error {
                color: red;
                font-size: 12px;
            }
        """)

        self.layout = QHBoxLayout()

        self.options_list = QListWidget()
        self.options_list.addItems(["Add Protocol", "Create Library"])
        self.options_list.setFixedWidth(200)
        self.options_list.setFont(QFont("Arial", 12))
        self.options_list.currentRowChanged.connect(self.change_option)

        self.right_widget = QStackedWidget()
        self.add_protocol_widget = self.create_add_protocol_widget()
        self.create_library_widget = self.create_library_widget()

        self.right_widget.addWidget(self.add_protocol_widget)
        self.right_widget.addWidget(self.create_library_widget)

        self.layout.addWidget(self.options_list)
        self.layout.addWidget(self.right_widget)
        self.setLayout(self.layout)

    def create_add_protocol_widget(self):
        widget = QWidget()
        layout = QVBoxLayout()

        protocol_name_label = QLabel("Protocol Name:")
        protocol_name_label.setFont(QFont("Arial", 12))
        self.protocol_name_input = QLineEdit()
        self.protocol_name_input.setPlaceholderText("Enter protocol name")
        self.protocol_name_error = QLabel("Protocol Name: Required")
        self.protocol_name_error.setObjectName("error")
        self.protocol_name_error.setVisible(False)

        protocol_type_label = QLabel("Protocol Type:")
        protocol_type_label.setFont(QFont("Arial", 12))
        self.protocol_type_dropdown = QComboBox()
        self.protocol_type_dropdown.addItems(["Request Type", "Response Type"])

        attributes_label = QLabel("Attributes:")
        attributes_label.setFont(QFont("Arial", 12))
        self.attributes_table = QTableWidget(1, 6)  # Start with 1 row by default
        self.attributes_table.setHorizontalHeaderLabels(
            ["Parameter", "Data Type", "Min Value", "Max Value", "Default"])
        self.attributes_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        # initial row with fixed buttons
        self.add_buttons_to_row(0)

        self.attribute_errors = [QLabel(f"{self.attributes_table.horizontalHeaderItem(i).text()}: Required") for i in range(self.attributes_table.columnCount() - 1)]
        for error_label in self.attribute_errors:
            error_label.setObjectName("error")
            error_label.setVisible(False)

        remarks_label = QLabel("Remarks:")
        remarks_label.setFont(QFont("Arial", 12))
        self.remarks_box = QTextEdit()

        generate_class_button = QPushButton("Generate Class")
        generate_class_button.clicked.connect(self.generate_class)

        bottom_button_layout = QHBoxLayout()
        bottom_button_layout.addWidget(generate_class_button)

        layout.addWidget(protocol_name_label)
        layout.addWidget(self.protocol_name_input)
        layout.addWidget(self.protocol_name_error)
        layout.addWidget(protocol_type_label)
        layout.addWidget(self.protocol_type_dropdown)
        layout.addWidget(attributes_label)
        layout.addWidget(self.attributes_table)
        for error_label in self.attribute_errors:
            layout.addWidget(error_label)
        layout.addWidget(remarks_label)
        layout.addWidget(self.remarks_box)
        layout.addLayout(bottom_button_layout)

        widget.setLayout(layout)
        return widget

    def add_buttons_to_row(self, row):
        button_layout = QHBoxLayout()

        add_button = QPushButton("+")
        add_button.setFixedWidth(30)
        add_button.setFixedHeight(20)
        add_button.clicked.connect(self.add_attribute)

        remove_button = QPushButton("×")
        remove_button.setFixedWidth(30)
        remove_button.setFixedHeight(20)
        remove_button.setEnabled(row != 0)  # Disabling remove button for the first row
        remove_button.clicked.connect(lambda _, row=row: self.remove_attribute(row))

        button_layout.addWidget(add_button)
        button_layout.addWidget(remove_button)

        action_widget = QWidget()
        action_widget.setLayout(button_layout)
        self.attributes_table.setCellWidget(row, 5, action_widget)

        # Add a dropdown for data types (misra standards)
        data_type_dropdown = QComboBox()
        data_types = [
            "bool", "char", "signed char", "unsigned char", "short", "unsigned short",
            "int", "unsigned int", "long", "unsigned long", "long long", "unsigned long long",
            "float", "double", "long double", "wchar_t", "char16_t", "char32_t"
        ]
        data_type_dropdown.addItems(data_types)
        data_type_dropdown.setEditable(True)  # searching
        self.attributes_table.setCellWidget(row, 1, data_type_dropdown)

    def add_attribute(self):
        row_position = self.attributes_table.rowCount()
        self.attributes_table.insertRow(row_position)
        self.add_buttons_to_row(row_position)

    def remove_attribute(self, row):
        if row != 0:  #  disabling removing the first row
            self.attributes_table.removeRow(row)

    def create_library_widget(self):
        widget = QWidget()
        layout = QVBoxLayout()

        libraries_label = QLabel("Available Classes:")
        libraries_label.setFont(QFont("Arial", 12))
        self.libraries_list = QListWidget()
        self.libraries_list.setFont(QFont("Arial", 10))
        self.libraries_list.setSelectionMode(QListWidget.MultiSelection)  # Allow multiple selections

        ok_button = QPushButton("OK")
        ok_button.clicked.connect(self.generate_library)

        layout.addWidget(libraries_label)
        layout.addWidget(self.libraries_list)
        layout.addWidget(ok_button)

        widget.setLayout(layout)
        return widget

    def change_option(self, index):
        self.right_widget.setCurrentIndex(index)
        if index == 1:  # If "Create Library" is selected
            self.generate_library_testing()

    def generate_class(self):
        # Reset error messages
        self.protocol_name_error.setVisible(False)
        for error_label in self.attribute_errors:
            error_label.setVisible(False)

        # Validate inputs
        if not self.protocol_name_input.text():
            self.protocol_name_error.setVisible(True)
            return

        # Collecting data from the table
        data = []
        for row in range(self.attributes_table.rowCount()):
            row_data = {}
            is_complete = True  

            for column in range(self.attributes_table.columnCount() - 1): 
                header_text = self.attributes_table.horizontalHeaderItem(column).text()
                if column == 1:  
                    data_type_widget = self.attributes_table.cellWidget(row, column)
                    if data_type_widget:
                        row_data[header_text] = data_type_widget.currentText()
                        if not data_type_widget.currentText():  
                            is_complete = False
                            self.attribute_errors[column].setVisible(True)
                else:
                    item = self.attributes_table.item(row, column)
                    if item:
                        row_data[header_text] = item.text()
                        if not item.text(): 
                            is_complete = False
                            self.attribute_errors[column].setVisible(True)
                    else:
                        is_complete = False
                        self.attribute_errors[column].setVisible(True)

            if is_complete: 
                mapped_obj = {key.lower().replace(" ", "_"): value for key, value in row_data.items()}
                mapped_obj["class_name"]= self.protocol_name_input.text()
                data.append(mapped_obj)
                

        # Convert data to JSON
        if not data:
            return
        
        json_data = json.dumps(data, indent=4)
        print("JSON Data to be sent:")
        print(json_data)

        # Sending JSON data to an API endpoint
        try:
            response = requests.post("http://127.0.0.1:8000/api/icd/", json=json_data)
            print("Response from API:", response.text)
            if response.status_code == 201:
                QMessageBox.information(self, "Success", "Data sent successfully!")
                # Adding protocol name to available classes list
                self.libraries_list.addItem(self.protocol_name_input.text())
            else:
                QMessageBox.warning(self, "Error", f"Failed to send data: {response.status_code}")
        except requests.exceptions.RequestException as e:
            QMessageBox.warning(self, "Error", f"An error occurred: {e}")

    def generate_library(self):
        # Get selected class names
        selected_items = self.libraries_list.selectedItems()
        selected_class_names = [item.text() for item in selected_items]

        if not selected_class_names:
            QMessageBox.warning(self, "Error", "No classes selected.")
            return

        # Convert data to JSON
        json_data = json.dumps(selected_class_names, indent=4)
        print("JSON Data to be sent:")
        print(json_data)

        # Sending JSON data to an API endpoint
        try:
            response = requests.post("http://127.0.0.1:8000/api/library/", json=json_data)
            print("Response from API:", response.text)
            if response.status_code == 201:
                QMessageBox.information(self, "Success", "Data sent successfully!")
                self.create_class_files(selected_class_names)  # Call the method to create files
            else:
                QMessageBox.warning(self, "Error", f"Failed to send data: {response.status_code}")
        except requests.exceptions.RequestException as e:
            QMessageBox.warning(self, "Error", f"An error occurred: {e}")

    def create_class_files(self, class_names):
        # Create directories if they don't exist
        base_dir = os.path.dirname(os.path.abspath(__file__))
        src_dir = os.path.join(base_dir, "src")
        inc_dir = os.path.join(base_dir, "inc")
        os.makedirs(src_dir, exist_ok=True)
        os.makedirs(inc_dir, exist_ok=True)

        for class_name in class_names:
            # Define file paths
            header_filename = os.path.join(inc_dir, f"{class_name}.h")
            source_filename = os.path.join(src_dir, f"{class_name}.cpp")

            # Create .h file
            with open(header_filename, "w") as header_file:
                header_file.write(f"#ifndef {class_name.upper()}_H\n")
                header_file.write(f"#define {class_name.upper()}_H\n\n")
                header_file.write("#include <string>\n\n")
                header_file.write(f"class {class_name} {{\n")
                header_file.write("public:\n")
                header_file.write(f"    {class_name}();\n\n")
                header_file.write("    // Add member variables here\n")
                header_file.write("};\n\n")
                header_file.write(f"#endif // {class_name.upper()}_H\n")

            # Create .cpp file
            with open(source_filename, "w") as source_file:
                source_file.write(f"#include \"{header_filename}\"\n\n")
                source_file.write(f"{class_name}::{class_name}() {{\n")
                source_file.write("    // Initialize member variables here\n")
                source_file.write("}\n")

    def generate_library_testing(self):
        # Receiving JSON data to an API endpoint
        try:
            # Define query parameters
            query_params = {"only_class": "true"}  # Pass 'only_class' as a query parameter
            
            # Make GET request with query parameters
            response = requests.get("http://127.0.0.1:8000/api/icd/", params=query_params)
            print("Response from API:", response.text)
            
            if response.status_code == 200:
                class_names = response.json()
                self.libraries_list.clear()  # Clear the list before adding new items
                self.libraries_list.addItems(class_names)
                QMessageBox.information(self, "Success", "Data received successfully!")
            else:
                QMessageBox.warning(self, "Error", f"Failed to receive data: {response.status_code}")
    
        except requests.exceptions.RequestException as e:
            QMessageBox.warning(self, "Error", f"An error occurred: {e}") 

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ICD Creation Tool")
        self.setFixedSize(800, 600)

        self.stacked_widget = QStackedWidget()

        self.login_page = LoginPage(self)
        self.main_page = MainPage()

        self.stacked_widget.addWidget(self.login_page)
        self.stacked_widget.addWidget(self.main_page)

        self.setCentralWidget(self.stacked_widget)

    def switch_to_main(self):
        self.stacked_widget.setCurrentWidget(self.main_page)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())