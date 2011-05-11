/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

package jmmc.mcs.err;

import java.util.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.io.File;

/**
 * <p>Titre : errEditor</p>
 * <p>Description : Errors Editor</p>
 * <p>Copyright : Copyright (c) 2004</p>
 * <p>Societe : LAOG</p>
 * @author Vittoz Fabien
 * @version 1.0
 */

public class ErrMenu
    extends JFrame
    implements Observer, WindowListener {
        JPanel mainPanel = new JPanel();
        JPanel editorPanel = new JPanel();
        JLabel errNameLabel = new JLabel();
        JTextField errNameTextField = new JTextField();
        JLabel errSeverityLabel = new JLabel();
        JLabel errFormatLabel = new JLabel();
        JButton addButton = new JButton();
        JButton delButton = new JButton();

        JMenuBar jMenuBar1 = new JMenuBar();
        JMenu fileMenu = new JMenu();
        JMenuItem newItem = new JMenuItem();
        JMenuItem openItem = new JMenuItem();
        JMenuItem openROItem = new JMenuItem();
        JMenuItem closeItem = new JMenuItem();
        JMenuItem quitItem = new JMenuItem();
        JMenu actionsMenu = new JMenu();
        JMenuItem generateItem = new JMenuItem();
        TitledBorder titledBorder1;
        JRadioButton warningRadioButton = new JRadioButton();
        JRadioButton severeRadioButton = new JRadioButton();
        JRadioButton fatalRadioButton = new JRadioButton();
        ButtonGroup g = new ButtonGroup();

        String modName = null;
        JScrollPane errorPanel = new JScrollPane();
        JTable errorListTable = new JTable();

        ErrTableModel tMod;
        ErrTableGenerator tableGen;
        ErrXMLGenerator xmlGen;
        JTextField errFormatTextField = new JTextField();
        JButton updateButton = new JButton();
        JMenuItem idModItem = new JMenuItem();
        JLabel modNameLabel = new JLabel();
        GridBagLayout gridBagLayout1 = new GridBagLayout();
        GridBagLayout gridBagLayout2 = new GridBagLayout();
        GridBagLayout gridBagLayout4 = new GridBagLayout();
        JTextField infosTextField = new JTextField();
        JButton clearButton = new JButton();

        String MCSROOT;
        String INTROOT;
        boolean ROmode = false; //indicates if the document was open in RO or not

        public ErrMenu(String introot, String mcsroot, boolean readOnlyMode) {
            try {
                this.addWindowListener(this);
                jbInit();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
            // init MCSROOT and INTROOT
            INTROOT=introot;
            MCSROOT=mcsroot;
            
            //Center the window
            Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
            Dimension frameSize = this.getSize();
            if (frameSize.height > screenSize.height) {
                frameSize.height = screenSize.height;
            }
            if (frameSize.width > screenSize.width) {
                frameSize.width = screenSize.width;
            }
            this.setLocation( (screenSize.width - frameSize.width) / 2,
                    (screenSize.height - frameSize.height) / 2);
            //this.setVisible(true);
            this.show();
            // $$$ End

            /**INITIALISATION**/
            initTable();

            //errTableGenerator initialisation + MVC mecanism (Observer/Observable)
            tableGen = new ErrTableGenerator(errorListTable);
            tableGen.addObserver(this);
            this.setUnactiveErrEditor();

	    // activate or not menu according ro mode
	    newItem.setEnabled(readOnlyMode);
	    openItem.setEnabled(readOnlyMode);
        }

        // get environment variable 
        public static Properties getEnvironment() throws java.io.IOException {
            Properties env = new Properties();
            env.load(Runtime.getRuntime().exec("env").getInputStream());

            return env;
        }   

        public static void main(String[] args) {
            /* get MODROOT and INTROOT environment variables */
            String introot = "";
            String mcsroot = "";
            try{
                Properties env = getEnvironment();
                introot = env.getProperty("INTROOT");
                mcsroot = env.getProperty("MCSROOT");
            } catch (Exception e){}

            /* check if ../errors exists */
            try{         
                File tmp = new File(".");
                File tmp2 = new File("../errors");
                if(tmp2.exists() && tmp2.isDirectory()){
                    ErrMenu errMenu = new ErrMenu(introot, mcsroot, true);
                    errMenu.dispStatus( "Your error file will be stored into : "
                            +tmp2.getCanonicalPath());
                }else{
                    System.out.println("WARNING : directory ../errors not found." +
                            "You will not be able to generate your error definition file"); 
                    ErrMenu errMenu = new ErrMenu(args[0], args[1],false);
                }
            }catch (Exception e){}
        }

        /** 
         * Display a message into the status bar
         * @param msg message to display
         */
        void dispStatus(String msg){
            infosTextField.setForeground(Color.blue);
            infosTextField.setText(msg);
        }

        /**
         * Display a warning message into the status bar
         * @param msg message to display
         */
        void dispWarningStatus(String msg){
            infosTextField.setForeground(Color.orange);
            infosTextField.setText(msg);
        }

        /** Display an error message into the status bar
         * @param msg message to display
         */
        void dispErrStatus(String msg){
            infosTextField.setForeground(Color.red);
            infosTextField.setText(msg);
        }


        /**
         * This method initialises the JTable with a specific JTableModel
         */
        private void initTable() {

            //Initialisation de la JTable
            tMod = new ErrTableModel(0, 4, new String[] {"Name", "Severity", "ID",
                "Format"});
            errorListTable.setModel(tMod);

            //errName
            errorListTable.getColumnModel().getColumn(0).setResizable(true);
            errorListTable.getColumnModel().getColumn(0).setPreferredWidth(200);
            //errSeverity
            errorListTable.getColumnModel().getColumn(1).setResizable(false);
            errorListTable.getColumnModel().getColumn(1).setPreferredWidth(70);
            errorListTable.getColumnModel().getColumn(1).setMinWidth(70);
            errorListTable.getColumnModel().getColumn(1).setMaxWidth(70);
            //id
            errorListTable.getColumnModel().getColumn(2).setResizable(false);
            errorListTable.getColumnModel().getColumn(2).setPreferredWidth(0);
            errorListTable.getColumnModel().getColumn(2).setMinWidth(0);
            errorListTable.getColumnModel().getColumn(2).setMaxWidth(0);
            //errFormat
            errorListTable.getColumnModel().getColumn(3).setResizable(true);
            errorListTable.getColumnModel().getColumn(3).setPreferredWidth(270);
            //ToolTip
            errorListTable.setToolTipText("Use the arrow key to move UP/DOWN a selected error");
            tMod.addTableModelListener(new TableModelListener() {
                public void tableChanged(TableModelEvent e) {
                    if (e.getType() == e.UPDATE) {
                        //If only the second column has changed
                        if (e.getColumn() == 2)
                            checkID(e.getFirstRow());
                    }
                }
            }
            );

        }

        /**
         * This method disables all buttons and textFields and clears all
         * of these items
         */
        private void setUnactiveErrEditor() {
            actionsMenu.setEnabled(false);
            errNameTextField.setEditable(false);
            errNameTextField.setText("");
            errFormatTextField.setEditable(false);
            errFormatTextField.setText("");
            addButton.setEnabled(false);
            delButton.setEnabled(false);
            updateButton.setEnabled(false);
            clearButton.setEnabled(false);
            //Radio Buttons
            warningRadioButton.setEnabled(false);
            severeRadioButton.setEnabled(false);
            fatalRadioButton.setEnabled(false);
            generateItem.setEnabled(false);
        }

        /**
         * This method enables all buttons and textFields
         */
        private void setActiveErrEditor() {
            actionsMenu.setEnabled(true);
            errNameTextField.setEditable(true);
            errFormatTextField.setEditable(true);
            addButton.setEnabled(true);
            delButton.setEnabled(true);
            updateButton.setEnabled(true);
            clearButton.setEnabled(true);
            //Radio Buttons
            warningRadioButton.setEnabled(true);
            severeRadioButton.setEnabled(true);
            fatalRadioButton.setEnabled(true);
            generateItem.setEnabled(true);
        }

        /**
         * This method records the new module name and prints it in the
         * corresponding label
         * @param modname String : the module name
         */
        private void setModuleName(String modname) {
            modName = modname;
            modNameLabel.setText(modName + "ERR_");
        }

        /**
         * This method erases the current module name and the corresponding label
         */
        private void unsetModuleName() {
            modName = null;
            modNameLabel.setText(null);
        }

        /**
         * This methods checks the error information, gets them and uses the
         * tableGenerator to complete the JTable
         */
        private void addError() {
            //First we verify if each data are mentionned
            if (errNameTextField.getText().equals("")) {
                dispErrStatus("You must enter an error name");
            }
            else if (errFormatTextField.getText().equals("")) {
                dispErrStatus("You must enter an error format");
            }
            else {
                String radioValue = null;
                Enumeration buttons = g.getElements();
                while (buttons.hasMoreElements()) {
                    JRadioButton but = (JRadioButton) buttons.nextElement();
                    if (but.isSelected()) {
                        radioValue = but.getText();
                        break;
                    }
                }
                //We add the moduleName to the error name with a _
                String res = tableGen.addError(modName + "ERR_" +
                        errNameTextField.getText().toUpperCase(),
                        radioValue, errFormatTextField.getText());
                if (res != null) {
                    dispErrStatus(res);
                }

            }
        }

        /**
         * This method is used when an error is selected in the table : its
         * information are set in the editor
         * @param row int : the error row
         */
        private void getError(int row) {
            errNameTextField.setText(tableGen.getErrName(row));
            errFormatTextField.setText(tableGen.getErrFormat(row));
            //Radio Buttons
            String severity = tableGen.getErrSeverity(row);
            String radioValue = null;
            Enumeration buttons = g.getElements();
            while (buttons.hasMoreElements()) {
                JRadioButton but = (JRadioButton) buttons.nextElement();
                if (but.getText().equals(severity)) {
                    but.setSelected(true);
                    break;
                }
            }
        }

        /**
         * This methods checks the error information, get them and uses the
         * errXMLGenerator to generate the XML file.
         * @return int : 1 if no problem occured else 0
         */
        private int generateXML() {
            if (modName != null) {
                int idErr = tableGen.checkAllId();
                if (idErr == -1) {
                    xmlGen = new ErrXMLGenerator(this, tableGen, modName);
                    if (xmlGen.generateXML("../errors") == 1) {
                        dispStatus("The XML file has been generated successfully");
                        return 1;
                    }
                    else {
                        return 0;
                    }
                }
                else if (idErr == -2) {
                    dispStatus("One (or more) id number is missing");
                    return 0;
                }
                else {
                    dispErrStatus("Check the id in the row " + idErr +
                            ". All id number must be different");
                    return 0;
                }
            }
            else {
                dispErrStatus("No file is currently opened !");
                return 0;
            }
        }

        /**
         * This method launches the XML parsing and table generation
         * @param path String : the path where the module name can be find
         * @return int : 0 if an error occured, else 1
         */
        private int readXML(String path) {
            xmlGen = new ErrXMLGenerator(this, tableGen, modName);
            if (xmlGen.readXML(path) == 1) {
                dispStatus("The file " + path+"/"+modName + "Errors.xml was successfully opened :)");
                return 1;
            }
            else {
                return 0;
            }
        }

        /**
         * This method closes the current file by clearing the file name and
         * disactivating the editor
         */
        private void closeCurrentFile() {
            if (ROmode){
                tableGen.clearTable();
                unsetModuleName();
                return;
            }
            int res = JOptionPane.showConfirmDialog(null,
                    "Do you want to generate the XML file ?",
                    "Exit errEditor",
                    JOptionPane.YES_NO_CANCEL_OPTION);
            if (res == JOptionPane.YES_OPTION) {
                if (generateXML() == 1)
                    //No error
                {
                    this.setUnactiveErrEditor();
                    tableGen.clearTable();
                    unsetModuleName();
                }
            }
            else if (res == JOptionPane.NO_OPTION) {
                //No save
                this.setUnactiveErrEditor();
                tableGen.clearTable();
                unsetModuleName();
            }
        }

	/**
	 * End the application.
	 */
	private void reallyQuit(){
		//Before ending, we ask the user if he wants to save
            if ((modName != null) && (ROmode == false)) {
                int res = JOptionPane.showConfirmDialog(null,
                        "Do you want to generate the XML file ?",
                        "Exit errEditor",
                        JOptionPane.YES_NO_CANCEL_OPTION);
                if (res == JOptionPane.YES_OPTION) {
                    if (generateXML() == 1)
                        this.dispose();
                }
                else if (res == JOptionPane.NO_OPTION) {
                    this.dispose();
                }
            }
            else
                this.dispose();
            System.exit(0);
	}
	
        /**
         * This method checks if the ID located in the row is unqiue
         * @param row int : the id row
         */
        private void checkID(int row) {
            if (tableGen.checkID(row) == 0) {
                dispErrStatus("Please check the id at the row number " + row +
                        ". All id number must be different");
            }
            else {
                dispStatus("All id are different");
            }
        }

        /**
         * This method allows (disallows) to modify the id number by hide (unhide)
         * the id column in the JTable
         */
        private void idModification() {
            if (errorListTable.getColumnModel().getColumn(2).getWidth() == 0)
                //the column is hidden
            {
                errorListTable.getColumnModel().getColumn(2).setPreferredWidth(30);
                errorListTable.getColumnModel().getColumn(2).setMinWidth(30);
                errorListTable.getColumnModel().getColumn(2).setMaxWidth(30);
                //We modify the label of the menu item
                idModItem.setText("No id modification");
            }
            else {
                errorListTable.getColumnModel().getColumn(2).setPreferredWidth(0);
                errorListTable.getColumnModel().getColumn(2).setMinWidth(0);
                errorListTable.getColumnModel().getColumn(2).setMaxWidth(0);
                idModItem.setText("Id modification");
            }
            tMod.fireTableDataChanged();
        }

        /**
         * This method open a XML document and call a function to generate the
         * JTable
         * @param path String : the module path
         */
        private void openDocument(String path) {
            //Before opening a new document we ask the user if he wants to save
            if ((modName != null) && (ROmode==false)) {
                this.closeCurrentFile();
            }

            if (modName == null) {
                String rep = JOptionPane.showInputDialog(this, "Module name : ",
                        "Open a XML document",
                        JOptionPane.QUESTION_MESSAGE);
                if (rep != null) {
                    if (!rep.equals("")) {
                        this.setModuleName(rep);
                        ROmode=false;
                        this.setActiveErrEditor();
                        if (readXML(path) == 0) {
                            unsetModuleName();
                            this.setUnactiveErrEditor();
                            tableGen.clearTable();
                        }
                    }
                    else {
                        dispWarningStatus("You must specify a file name");
                    } //else
                } //if
            } //if
        }
	/**
	 * This method open a read only XML document and call a function to generate the
	 * JTable
	 * @param path String : the module path
	 */
	private void openRODocument(String path) {
		if (readXML(path) == 0) {
			unsetModuleName();
			this.setUnactiveErrEditor();
			tableGen.clearTable();
		}else{
			this.setUnactiveErrEditor();
			ROmode=true;
		}
	}

	/**
	 * This funtion gets the error informations, checks them, and udpates
         * the JTable
         */
        private void updateError() {
            //Data verification
            if (errNameTextField.getText().equals("")) {
                dispErrStatus("You must enter an error name");
            }
            else if (errFormatTextField.getText().equals("")) {
                dispErrStatus("You must enter an error format");
            }
            else {
                String radioValue = null;
                Enumeration buttons = g.getElements();
                while (buttons.hasMoreElements()) {
                    JRadioButton but = (JRadioButton) buttons.nextElement();
                    if (but.isSelected()) {
                        radioValue = but.getText();
                        break;
                    }
                }
                //We update the errorName
                String res = tableGen.updateError(errNameTextField.getText(),
                        radioValue, errFormatTextField.getText());
                if (res != null) {
                    dispErrStatus(res);
                }
                else {
                    dispStatus("Error updated with success");
                }
            }
        }

        /**
         * This method initialises the window
         * @throws Exception
         */
        private void jbInit() throws Exception {
            titledBorder1 = new TitledBorder("");
            this.setJMenuBar(jMenuBar1);
            this.setSize(new Dimension(800, 750));
            this.setTitle("ErrEditor");
            this.getContentPane().setLayout(gridBagLayout1);
            mainPanel.setBackground(Color.lightGray);
            mainPanel.setAlignmentX( (float) 0.5);
            mainPanel.setLayout(gridBagLayout2);
            editorPanel.setBackground(SystemColor.activeCaptionBorder);
            editorPanel.setDebugGraphicsOptions(0);
            editorPanel.setMaximumSize(new Dimension(500, 2147483647));
            editorPanel.setOpaque(true);
            editorPanel.setPreferredSize(new Dimension(500, 446));
            editorPanel.setLayout(gridBagLayout4);
            errNameLabel.setText("Error name :");
            errNameTextField.setText("");
            errNameTextField.addActionListener(new
                    ErrMenu_errNameTextField_actionAdapter(this));
            errSeverityLabel.setText("Error severity :");
            errFormatLabel.setText("Error format :");//To return automatically at the line//The words are not cut
            addButton.setMnemonic('0');
            addButton.setText("ADD");
            addButton.addActionListener(new ErrMenu_addButton_actionAdapter(this));
            delButton.setMaximumSize(new Dimension(63, 34));
            delButton.setMinimumSize(new Dimension(63, 34));
            delButton.setPreferredSize(new Dimension(63, 34));
            delButton.setText("DEL");
            delButton.addActionListener(new ErrMenu_delButton_actionAdapter(this));
            fileMenu.setText("File");
            newItem.setText("New...");
            newItem.addActionListener(new ErrMenu_newItem_actionAdapter(this));
            openItem.setText("Open...");
            openItem.addActionListener(new ErrMenu_openItem_actionAdapter(this));
            openROItem.setText("Open (Read Only)...");
            openROItem.addActionListener(new ErrMenu_openROItem_actionAdapter(this));
            closeItem.setText("Close");
            closeItem.addActionListener(new ErrMenu_closeItem_actionAdapter(this));
            quitItem.setText("Quit");
            quitItem.addActionListener(new ErrMenu_quitItem_actionAdapter(this));
            actionsMenu.setText("Actions");
            generateItem.setToolTipText("");
            generateItem.setSelectedIcon(null);
            generateItem.setText("Generate XML File");
            generateItem.addActionListener(new ErrMenu_generateItem_actionAdapter(this));
            //Radio Buttons
            warningRadioButton.setSelected(true);
            warningRadioButton.setText("WARNING");
            severeRadioButton.setSelected(true);
            severeRadioButton.setText("SEVERE");
            fatalRadioButton.setEnabled(true);
            fatalRadioButton.setText("FATAL");

            errorPanel.setHorizontalScrollBarPolicy(JScrollPane.
                    HORIZONTAL_SCROLLBAR_NEVER);
            errorPanel.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
            errorPanel.getViewport().setBackground(Color.white);
            errorListTable.addMouseListener(new ErrMenu_errorListTable_mouseAdapter(this));
            errFormatTextField.addActionListener(new
                    ErrMenu_errFormatTextField_actionAdapter(this));
            errFormatTextField.setText("");
            updateButton.setText("UPDATE");
            updateButton.addActionListener(new ErrMenu_updateButton_actionAdapter(this));
            idModItem.setText("Id modification");
            idModItem.setArmed(false);
            idModItem.addActionListener(new ErrMenu_idModItem_actionAdapter(this));
            errorListTable.addKeyListener(new ErrMenu_errorListTable_keyAdapter(this));
            infosTextField.setBackground(Color.white);
            infosTextField.setEnabled(true);
            infosTextField.setFont(new java.awt.Font("Dialog", 0, 12));
            infosTextField.setForeground(Color.red);
            infosTextField.setBorder(BorderFactory.createLineBorder(Color.black));
            infosTextField.setCaretColor(Color.black);
            infosTextField.setEditable(false);
            modNameLabel.setMaximumSize(new Dimension(90, 24));
            modNameLabel.setMinimumSize(new Dimension(90, 24));
            modNameLabel.setHorizontalAlignment(SwingConstants.RIGHT);
            modNameLabel.setText("modNameERR_");
            // errorListTable.setBackground(Color.white);
            errorListTable.setGridColor(Color.white);
            clearButton.addActionListener(new ErrMenu_clearButton_actionAdapter(this));
            clearButton.setText("CLEAR");
            clearButton.addActionListener(new ErrMenu_clearButton_actionAdapter(this));
            g.add(warningRadioButton);
            g.add(severeRadioButton);
            g.add(fatalRadioButton);
            //End Radio Buttons

            editorPanel.add(delButton,                                                                          new GridBagConstraints(0, 5, 3, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(15, 300, 10, 0), 10, -1));
            editorPanel.add(errFormatTextField,                      new GridBagConstraints(2, 4, 1, 1, 1.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, new Insets(15, 4, 0, 4), 173, -6));
            editorPanel.add(errNameLabel,                                     new GridBagConstraints(0, 1, 2, 1, 0.0, 0.0
                        ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(10, 4, 0, 0), 48, -3));
            editorPanel.add(errNameTextField,                                                                                                         new GridBagConstraints(2, 2, 1, 1, 1.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, new Insets(1, 4, 0, 4), 173, -6));
            editorPanel.add(errSeverityLabel,                           new GridBagConstraints(0, 3, 2, 1, 0.0, 0.0
                        ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(10, 4, 0, 0), 48, -3));
            editorPanel.add(errFormatLabel,              new GridBagConstraints(0, 4, 2, 1, 0.0, 0.0
                        ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(15, 4, 0, 0), 48, -3));
            editorPanel.add(warningRadioButton,                                                       new GridBagConstraints(2, 3, 1, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(10, -430, 5, 0), 28, -8));
            editorPanel.add(severeRadioButton,                       new GridBagConstraints(2, 3, 1, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(10, -130, 5, 0), 33, -8));
            editorPanel.add(fatalRadioButton,                          new GridBagConstraints(2, 3, 1, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(10, 0, 5, -170), 49, -8));
            editorPanel.add(updateButton,                                                new GridBagConstraints(0, 5, 3, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(15, 0, 10, 0), 10, -1));
            editorPanel.add(addButton,                                                                                                  new GridBagConstraints(0, 5, 3, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(15, 0, 10, 300), 10, -1));
            editorPanel.add(modNameLabel,                                                                                      new GridBagConstraints(0, 2, 2, 1, 0.0, 0.0
                        ,GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(1, 0, 0, 0), 48, -3));
            editorPanel.add(clearButton,           new GridBagConstraints(0, 5, 3, 1, 0.0, 0.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(15, 0, 10, 680), 10, -1));
            mainPanel.add(editorPanel,                                                                             new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0
                        ,GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 10, 0), 0, -20));
            mainPanel.add(errorPanel,                                          new GridBagConstraints(0, 1, 1, 1, 1.0, 1.0
                        ,GridBagConstraints.SOUTH, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
            mainPanel.add(infosTextField,                    new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0
                        ,GridBagConstraints.SOUTH, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
            errorPanel.getViewport().add(errorListTable, null);
            jMenuBar1.add(fileMenu);
            jMenuBar1.add(actionsMenu);
            fileMenu.add(newItem);
            fileMenu.addSeparator();
            fileMenu.add(openItem);
            fileMenu.add(openROItem);
            fileMenu.addSeparator();
            fileMenu.add(generateItem);
            fileMenu.add(closeItem);
            fileMenu.addSeparator();
            fileMenu.add(quitItem);
            actionsMenu.addSeparator();
            actionsMenu.add(idModItem);
            this.getContentPane().add(mainPanel,  new GridBagConstraints(0, 0, 1, 1, 1.0, 1.0
                        ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 809, 551));
            errorListTable.getTableHeader().setReorderingAllowed(false);
            errorListTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            this.setDefaultCloseOperation(EXIT_ON_CLOSE);
        }

        void quitItem_actionPerformed(ActionEvent e) {
            reallyQuit();
	}

        void newItem_actionPerformed(ActionEvent e) {
            //Before opening a new document we ask the user if he wants to save
            if (modName != null) {
                this.closeCurrentFile();
            }

            if (modName == null) {
                String rep = JOptionPane.showInputDialog(this, "Module name : ",
                        "Create document",
                        JOptionPane.QUESTION_MESSAGE);
                if (rep != null) {
                    if (!rep.equals("")) {
                        File tmp = new File(rep+"Errors.xml");
                        if(tmp.exists()){
                            dispErrStatus("File already exists");
                        }else{
                            this.setModuleName(rep);
                            this.setActiveErrEditor();
                            dispStatus("Enjoy errEditor ;)");
                        }
                    }
                    else {
                        dispWarningStatus("You must specify a file name");
                    } //else
                } //if
            } //if
        } //function


        void closeItem_actionPerformed(ActionEvent e) {
            closeCurrentFile();
        }

        void addButton_actionPerformed(ActionEvent e) {
            addError();
        }

        /**
         * update
         *
         * @param o Observable
         * @param arg Object
         */
        public void update(Observable o, Object arg) {
            tMod.fireTableDataChanged();
            dispStatus("The error has been had to the list");
        }

        void errNameTextField_actionPerformed(ActionEvent e) {
            addError();
        }

        void errorListTable_mouseClicked(MouseEvent e) {
            getError(errorListTable.getSelectedRow());
        }

        void generateItem_actionPerformed(ActionEvent e) {
            generateXML();
        }

        void delButton_actionPerformed(ActionEvent e) {

            if (errorListTable.getSelectedRow() != -1) {
                tableGen.delError(errorListTable.getSelectedRow());
                dispStatus("The error has been deleted");
            }
        }

        void openItem_actionPerformed(ActionEvent e) {
            openDocument("../errors");
        }

        void errFormatTextField_actionPerformed(ActionEvent e) {
            addError();
        }

        void idModItem_actionPerformed(ActionEvent e) {
            this.idModification();
        }
        /** 
         * Search into INTROOT or into MCSROOT if not found into INTROOT.
         *
         */
        void openROItem_actionPerformed(ActionEvent e) {
            //Before opening a new document we ask the user if he wants to save
            if (modName != null) {
                this.closeCurrentFile();
            }

            String rep = JOptionPane.showInputDialog(this, "Module name : ",
                    "Open a XML document",
                    JOptionPane.QUESTION_MESSAGE);
            if (rep != null) {
                if (!rep.equals("")) {
                    this.setModuleName(rep);
                } else {
                    dispWarningStatus("You must specify a file name");
                    return;
                } //else
            } //if

            try{
                File tmp = new File(INTROOT+"/errors/"+rep+"Errors.xml");
                if (tmp.exists()){
                    openRODocument(INTROOT+"/errors");
                    return;
                }
                tmp = new File(MCSROOT+"/errors/"+rep+"Errors.xml");
                if (tmp.exists()){
                    openRODocument(MCSROOT+"/errors");
                    return;
                }
            }catch(Exception exc){}

            dispErrStatus("No error description file found into INTROOT nor"+
                    " MCSROOT");

        }

        void updateButton_actionPerformed(ActionEvent e) {
            updateError();
        }

        void errorListTable_keyPressed(KeyEvent e) {
            if (e.getKeyCode() == KeyEvent.VK_UP) {
                tableGen.moveRow(errorListTable.getSelectedRow(),
                        errorListTable.getSelectedRow() - 1);
            }
            else if (e.getKeyCode() == KeyEvent.VK_DOWN) {
                tableGen.moveRow(errorListTable.getSelectedRow(),
                        errorListTable.getSelectedRow() + 1);
            }
        }

        void clearButton_actionPerformed(ActionEvent e) {
            errNameTextField.setText("");
            errFormatTextField.setText("");
        }

        /**
         * windowActivated
         *
         * @param e WindowEvent
         */
        public void windowActivated(WindowEvent e) {
        }

        /**
         * windowClosed
         *
         * @param e WindowEvent
         */
        public void windowClosed(WindowEvent e) {
        }

        /**
         * windowClosing
         *
         * @param e WindowEvent
         */
        public void windowClosing(WindowEvent e) {
        	reallyQuit();
	}

        /**
         * windowDeactivated
         *
         * @param e WindowEvent
         */
        public void windowDeactivated(WindowEvent e) {
        }

        /**
         * windowDeiconified
         *
         * @param e WindowEvent
         */
        public void windowDeiconified(WindowEvent e) {
        }

        /**
         * windowIconified
         *
         * @param e WindowEvent
         */
        public void windowIconified(WindowEvent e) {
        }

        /**
         * windowOpened
         *
         * @param e WindowEvent
         */
        public void windowOpened(WindowEvent e) {
        }

} //end ErrMenu

class ErrMenu_quitItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_quitItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.quitItem_actionPerformed(e);
        }
}

class ErrMenu_newItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_newItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.newItem_actionPerformed(e);
        }
}

class ErrMenu_closeItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_closeItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.closeItem_actionPerformed(e);
        }
}

class ErrMenu_addButton_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_addButton_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.addButton_actionPerformed(e);
        }
}

class ErrMenu_errNameTextField_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_errNameTextField_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.errNameTextField_actionPerformed(e);
        }
}

class ErrMenu_errorListTable_mouseAdapter
    extends java.awt.event.MouseAdapter {
        ErrMenu adaptee;

        ErrMenu_errorListTable_mouseAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void mouseClicked(MouseEvent e) {
            adaptee.errorListTable_mouseClicked(e);
        }
}

class ErrMenu_generateItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_generateItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.generateItem_actionPerformed(e);
        }
}

class ErrMenu_delButton_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_delButton_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.delButton_actionPerformed(e);
        }
}

class ErrMenu_openItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_openItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.openItem_actionPerformed(e);
        }
}

class ErrMenu_errFormatTextField_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_errFormatTextField_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.errFormatTextField_actionPerformed(e);
        }
}

class ErrMenu_idModItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_idModItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.idModItem_actionPerformed(e);
        }
}

class ErrMenu_openROItem_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_openROItem_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.openROItem_actionPerformed(e);
        }
}

class ErrMenu_updateButton_actionAdapter
    implements java.awt.event.ActionListener {
        ErrMenu adaptee;

        ErrMenu_updateButton_actionAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void actionPerformed(ActionEvent e) {
            adaptee.updateButton_actionPerformed(e);
        }
}

class ErrMenu_errorListTable_keyAdapter
    extends java.awt.event.KeyAdapter {
        ErrMenu adaptee;

        ErrMenu_errorListTable_keyAdapter(ErrMenu adaptee) {
            this.adaptee = adaptee;
        }

        public void keyPressed(KeyEvent e) {
            adaptee.errorListTable_keyPressed(e);
        }
}

class ErrMenu_clearButton_actionAdapter implements java.awt.event.ActionListener {
    ErrMenu adaptee;

    ErrMenu_clearButton_actionAdapter(ErrMenu adaptee) {
        this.adaptee = adaptee;
    }
    public void actionPerformed(ActionEvent e) {
        adaptee.clearButton_actionPerformed(e);
    }
}
