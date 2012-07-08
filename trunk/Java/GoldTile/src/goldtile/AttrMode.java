/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package goldtile;
public enum AttrMode {
    ATTR_MODE_ABC, 
    ATTR_MODE_RST, 
    ATTR_MODE_123, 
    ATTR_MODE_SHAPE,
    ATTR_MODE_COLOR;
    
    
    /**
     * @param value the value of the attribute
     */
    public char attrToChar(int value) {
        char result = '?';
        
        switch (this) {
            case ATTR_MODE_ABC:
                result = (char)('A' + value);
                break;
            case ATTR_MODE_RST:
                result = (char)('R' + value);
                break;
            case ATTR_MODE_123:
                result = (char)('1' + value);
                break;
            default:
                throw new RuntimeException();
        }

        return result;
    }

    /**
     * @param value the value of the attribute
     */
    public int charToAttr(char ch) {
        int result = 0;
        
        switch (this) {
            case ATTR_MODE_ABC:
                result = (int)(ch - 'A');
                break;
            case ATTR_MODE_RST:
                result = (int)(ch - 'R');
                break;
            case ATTR_MODE_123:
                result = (int)(ch - '1');
                break;
            default:
                throw new RuntimeException();
        }

        return result;
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static AttrMode defaultDisplayMode(int iAttr) {
        AttrMode result = ATTR_MODE_SHAPE;
        
        switch (iAttr) {
            case 0:
                result = ATTR_MODE_ABC;
                break;
            case 1:
                result = ATTR_MODE_RST;
                break;
            default:
                result = ATTR_MODE_123;
        }
        
        return result;
    }
}
