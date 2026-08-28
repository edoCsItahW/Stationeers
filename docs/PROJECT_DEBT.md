# Project Debt

<details>
    <summary>中文</summary>
    并不是一个严肃的文件，任何开发者都可以当作备忘录使用，用于向其它开发者示意（主要为审核者）项目中还有哪些事物留待下次处理
    遵循格式

    ```
    <开发者姓名、通讯名或邮箱>:
        <债务名> {选填的预期完成时间}
            <债务描述（主要描述目标板块）>
    ```

    可以配合git提交信息使用，例如：
    ```
    feat: some feature
    
    DEBT:
        - <上述债务名>
    ```
</details>


This is not a formal document; any developer may use it as a memo to indicate to other developers (primarily reviewers) what items in the project remain to be addressed in the next iteration.

Follow the format:
```
<Developer name, handle, or email>:
    <Debt name> {optional expected completion date}
        <Debt description (mainly describing the target area/module)>
```

It can be used in conjunction with Git commit messages, for example:
```
feat: some feature

DEBT:
    - <debt name as above>
```

## Project Debt

edocsitahw:  
    IC10 code documentation  
        Fix and improve the broken code documentation
