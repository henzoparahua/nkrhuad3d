```mermaid
%%{init: { 'themeVariables': { 'clusterBkg': 'transparent', 'clusterBorder': 'transparent'}}}%%
flowchart TB
subgraph Core
CoreMain["Main"]
end
CoreMain --> WinSystem
subgraph Win
	WinSystem["SystemClass"]
	WinSystem --> WinApplication["Application Class"]
	WinSystem --> WinInput["Input Class"]
end
```
