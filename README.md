### mm32f5270_nano_frame

#### 关于密钥生成

```
1 生成密钥
ssh-keygen -t ed25519 -C "你的邮箱@example.com"

2 启动 SSH 代理
eval "$(ssh-agent -s)"

3 添加私钥
ssh-add ~/.ssh/id_ed25519

4 将公钥复制到GitHub：
cat ~/.ssh/id_ed25519.pub
```

#### 关于密钥子模块调用

`git submodule add https://github.com/wodecoin/mm32f5270_nano_frame.git mm32f5270_nano_frame`

`git submodule update --remote`

