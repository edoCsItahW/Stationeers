package org.stationeers;

import com.intellij.DynamicBundle;
import org.jetbrains.annotations.Nls;
import org.jetbrains.annotations.PropertyKey;


import java.util.function.Supplier;

final class MyMessageBundle {

    private static final String BUNDLE = "messages.MyMessageBundle";

    private static final DynamicBundle INSTANCE = new DynamicBundle(MyMessageBundle.class, BUNDLE);

    private MyMessageBundle() {
    }

    static @Nls String message(@PropertyKey(resourceBundle = BUNDLE) String key, Object... params) {
        return INSTANCE.getMessage(key, params);
    }

    static Supplier<@Nls String> lazyMessage(@PropertyKey(resourceBundle = BUNDLE) String key, Object... params) {
        return INSTANCE.getLazyMessage(key, params);
    }
}
