/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

package jmmc.mcs.err;

import javax.swing.table.*;

public class ErrTableModel
    extends DefaultTableModel {
  String[] Header;

  /**
   * This constructor builds a l*c table with the header as column name
   * @param l int : the number of rows
   * @param c int : the number of columns
   * @param header String[] : the column names
   */
  public ErrTableModel(int l, int c, String[] header) {
    super(l, c);
    Header = header;
  }

  /**
   * This method returns a column name
   * @param col int : the column number
   * @return String : the column name
   */
  public String getColumnName(int col) {
    return Header[col];
  }

  /**
   * Ths method returns the object class
   * @param c int : the column number
   * @return Class : the object class
   */
  public Class getColumnClass(int c) {
    try {
      return getValueAt(0, c).getClass();
    }
    catch (Exception ex) {
      return "".getClass();
    }
  }

  /**
   * This method adds a new row to the table
   */
  public void addRow() {
    Object[] newRow = new Object[4];
    this.addRow(newRow);
  }

  /**
   * This methods is used to know which cells are editable
   * @param row int : the row number
   * @param column int : the column number
   * @return boolean : true if the cell is editable else false
   */
  public boolean isCellEditable(int row, int column) {
    if (column == 2)
      return true; //It is possible to edit the id value
    else
      return false;
  }

}
