// Created by Jakub Lukac on 17.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

/**
 * Immutable tuple consisted of value and label.
 *
 * @param <T> type of item value
 */
public class ComplexItem<T> {
    private T item;
    private String label;

    public ComplexItem(T item, String label){
        this.item = item;
        this.label = label;
    }

    public T getItem() {
        return item;
    }

    @Override
    public String toString() {
        return label;
    }
}
