package org.stationeers;

import com.intellij.openapi.project.Project;
import com.intellij.openapi.wm.ToolWindow;
import com.intellij.openapi.wm.ToolWindowFactory;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBPanel;
import com.intellij.ui.content.ContentFactory;

import javax.swing.JButton;
import java.util.Random;

public class MyToolWindowFactory implements ToolWindowFactory {

    @Override
    public boolean shouldBeAvailable(Project project) {
        return true;
    }

    @Override
    public void createToolWindowContent(Project project, ToolWindow toolWindow) {
        MyToolWindow myToolWindow = new MyToolWindow();
        var content = ContentFactory.getInstance().createContent(myToolWindow.getContent(), null, false);
        toolWindow.getContentManager().addContent(content);
    }

    static class MyToolWindow {

        private final JBPanel<JBPanel<?>> content;

        MyToolWindow() {
            content = new JBPanel<>();
            var label = new JBLabel(MyMessageBundle.message("toolwindow.MyToolWindow.number.label", "?"));
            content.add(label);

            var button = new JButton(MyMessageBundle.message("toolwindow.MyToolWindow.shuffle.button"));
            button.addActionListener(e -> label.setText(MyMessageBundle.message(
                    "toolwindow.MyToolWindow.number.label",
                    new Random(System.currentTimeMillis()).nextInt(1000))));
            content.add(button);
        }

        JBPanel<JBPanel<?>> getContent() {
            return content;
        }
    }
}
