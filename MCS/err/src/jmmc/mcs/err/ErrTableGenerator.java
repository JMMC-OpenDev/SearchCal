package jmmc.mcs.err;

import java.util.*;

import javax.swing.*;
import javax.swing.table.*;

public class ErrTableGenerator
    extends Observable {
  JTable errorTable;
  int line;
  int ID;

  public ErrTableGenerator(JTable errortable) {
    errorTable = errortable;
    line = 0;
    ID = 1;
  }

  /**
   * This method adds the error informations (parameters) in the table
   * @param errName String : the error name
   * @param errSeverity String : the error severity
   * @param errFormat String : the error format
   * @return String : nulll if no error, else an error message
   */
  public String addError(String errName, String errSeverity, String errFormat) {
    if (verifyErrName(errName)) {
      if (line == errorTable.getRowCount()) {
        ErrTableModel tMod = (ErrTableModel) errorTable.getModel();
        tMod.addRow();
      }
      errorTable.setValueAt(errName, line, 0);
      errorTable.setValueAt(errSeverity, line, 1);

      Integer id = new Integer(ID);
      errorTable.setValueAt(id, line, 2);
      errorTable.setValueAt(errFormat, line, 3);
      line++;
      ID++;
      //Mécanisme MVC
      setChanged(); //Mecanisme MVC
      notifyObservers();

      return null; //No problem
    }
    else {
      return new String("An existant error has the same name : you must specify a unique name for each error");
    }

  }

  /**
   * This method is the same than the other but it allows to specify an id
   * number
   * @param errName String : the error name
   * @param errSeverity String : the error severity
   * @param errFormat String : the error format
   * @param id int : the error id
   * @return String : null if no error, else an error message
   */
  public String addError(String errName, String errSeverity, String errFormat,
                         int id) {
    if (verifyErrName(errName)) {
      if (line == errorTable.getRowCount()) {
        ErrTableModel tMod = (ErrTableModel) errorTable.getModel();
        tMod.addRow();
      }
      errorTable.setValueAt(errName, line, 0);
      errorTable.setValueAt(errSeverity, line, 1);

      Integer i = new Integer(id);
      errorTable.setValueAt(i, line, 2);
      errorTable.setValueAt(errFormat, line, 3);
      line++;
      modifyID(id);
      //Mécanisme MVC
      setChanged(); //Mecanisme MVC
      notifyObservers();

      return null; //No problem
    }
    else {
      return new String("An existant error has the same name : you must specify a unique name for each error");
    }
  }

  /**
   * This method is used when a XML file is injected in the Table. It allows
   * to stock in the id the last value entered in the Table in order to avoid
   * to have 2 identique values
   * @param id int : the id value which must be compared to the current id
   */
  private void modifyID(int id) {
    if (id >= ID)
      ID = id + 1;
  }

  /**
   * This method checks if an error name is unique or not
   * @param errName String : the error name which must be compred with the
   * others
   * @return boolean : true if errName doesn't exist in the table else false
   */
  private boolean verifyErrName(String errName) {
    String errNameTable = null;
    for (int i = 0; i < line; i++) {
      errNameTable = (String) errorTable.getValueAt(i, 0);
      if (errNameTable.equals(errName))
        return false;
    }

    return true;
  }

  /**
   * This method deletes a row in the table
   * @param row int : the row number to delete
   */
  public void delError(int row) {
    if (row == line - 1)
    //Last table line
    {
      clearRow(row);
    }
    else {
      int j = row + 1;
      for (j = row + 1; j < line; j++) {
        copyRow(j, j - 1);
      }
      clearRow(line - 1);
    }
    line--;
    //Mécanisme MVC
    setChanged(); //Mecanisme MVC
    notifyObservers();
  }

  /**
   * This method clears the datas of a row
   * @param row int : the row number
   */
  private void clearRow(int row) {
    //errName
    errorTable.setValueAt(null, row, 0);
    //errSeverity
    errorTable.setValueAt(null, row, 1);
    //id
    errorTable.setValueAt(null, row, 2);
    //errFormat
    errorTable.setValueAt(null, row, 3);
  }

  /**
   * This method copies a row
   * @param src int : the source row
   * @param des int : the destination row
   */
  private void copyRow(int src, int des) {
    //errName
    errorTable.setValueAt(errorTable.getValueAt(src, 0), des, 0);
    //errSeverity
    errorTable.setValueAt(errorTable.getValueAt(src, 1), des, 1);
    //id
    errorTable.setValueAt(errorTable.getValueAt(src, 2), des, 2);
    //errFormat
    errorTable.setValueAt(errorTable.getValueAt(src, 3), des, 3);
  }

  /**
   * This methods returns an error name locating in a row number
   * @param row int : the row number
   * @return String : the error name
   */
  public String getErrName(int row) {
    if (row < line)
      return delModName( (String) errorTable.getValueAt(row, 0), "_");
    else
      return null;
  }

  /**
   * This methods returns an error severity locating in a row number
   * @param row int : the row number
   * @return String : the error severity
   */
  public String getErrSeverity(int row) {
    if (row < line)
      return (String) errorTable.getValueAt(row, 1);
    else
      return null;
  }

  /**
   * This methods returns an id locating in a row number
   * @param row int : the row number
   * @return Integer : the id number
   */
  public Integer getId(int row) {
    if (row < line) {
      return (Integer) errorTable.getValueAt(row, 2);
    }
    else
      return null;
  }

  /**
   * This methods returns an error format locating in a row number
   * @param row int : the row number
   * @return String : the error format
   */
  public String getErrFormat(int row) {
    if (row < line)
      return (String) errorTable.getValueAt(row, 3);
    else
      return null;
  }

  /**
   * This methods returns the number of rows which contains datas
   * @return int : the number of rows
   */
  public int getRowCount() {
    return line;
  }

  /**
   * This method returns the first substring of str delimiting by del
   * @param str String : the string to analyse
   * @param del String : the delimiter
   * @return String : the result string
   */
  private String delModName(String str, String del) {
    int pos = str.indexOf('_');
    String res = null;
    if (pos !=-1)
    {
      res = str.substring(pos+1);
    }
    return res;
  }

  /**
   * This method deletes all datas in the table
   */
  public void clearTable() {
    for (int i = 0; i < line; i++) {
      this.clearRow(i);
    }
    line = 0;
    ID = 1;
  }

  /**
   * This method compares the id located in the row 'row' with the others
   * @param row int : the row number
   * @return int : 0 if 2 id are equals else 1
   */
  public int checkID(int row) {
    Integer idRow = (Integer) errorTable.getValueAt(row, 2);
    //Id == null when the table is being clear
    if (idRow != null) {
      Integer id;
      for (int i = 0; i < line; i++) {
        if (i != row) {
          id = (Integer) errorTable.getValueAt(i, 2);
          //id == null when the table is being clear
          if (id != null) {
            if (id.equals(idRow))
              return 0; //2 id equals
          }
        }
      }
    }
    return 1; //All id different
  }

  /**
   * This method compares all id values
   * @return int : -1 if all id are different or the row number of the first id
   * which is equals to another, or -2 if one id number is missing
   */
  public int checkAllId() {
    Integer idi;
    Integer idj;

    for (int i = 0; i < line; i++) {
      idi = (Integer) errorTable.getValueAt(i, 2);
      if (idi == null)
        return -2;
      for (int j = i + 1; j < line; j++) {
        idj = (Integer) errorTable.getValueAt(j, 2);
        if (idj == null)
          return -2;
        if (idi.equals(idj))
          return i; //2 id equals
      }
    }
    return -1; //All id different

  }

  /**
   * This method allows to update an existing error
   * @param errName String : the error name which must be updated
   * @param errSeverity String : the error severity
   * @param errFormat String : the error format
   * @return String : null if no errors, else an error message
   */
  public String updateError(String errName, String errSeverity,
                            String errFormat) {
    String name;
    for (int i = 0; i < line; i++) {
      name = (String) errorTable.getValueAt(i, 0);
      name = this.delModName(name, "_");
      if (name.equals(errName)) {
        errorTable.setValueAt(errSeverity, i, 1);
        errorTable.setValueAt(errFormat, i, 3);
        return null;
      }
    }
    //No name corresponding to errName : no update done
    return new String("No entry for the selected name : '" + errName + "'");
  }

  /**
   * This method permuts 2 rows
   * @param src int : the source row number
   * @param des int : the destination row number
   */
  public void moveRow(int src, int des) {
    if (src >= 0 && src < line && des >= 0 && des < line) {
      DefaultTableModel tMod = (DefaultTableModel) errorTable.getModel();
      tMod.moveRow(src, src, des);
    }
  }
}
